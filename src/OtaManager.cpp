/**
 * @file OtaManager.cpp
 * @brief Implementation of the OtaManager class for OTA updates and system validation.
 *
 * This file contains the implementation of the OtaManager class, which handles
 * Over-The-Air (OTA) firmware updates, LittleFS initialization, LED status indication,
 * and system validation.
 */

#include "OtaManager.h"
#include "config.h"

namespace MyProject {

static const char *TAG = "OtaManager";

/**
 * @brief Constructor for OtaManager.
 *
 * Initializes the OTA task handle.
 */
OtaManager::OtaManager() : m_led_task_handle(NULL) {}

/**
 * @brief Starts the OTA and LED tasks.
 */
void OtaManager::start() {
    ESP_LOGI(TAG, "Memulai layanan OTA...");
    xTaskCreate(otaTaskRunner, "ota_task", 4096, this, 5, NULL);
    xTaskCreate(ledTaskRunner, "led_task", 1024, this, 5, &m_led_task_handle);
}

/**
 * @brief Initializes LittleFS and validates the system.
 */
void OtaManager::initializeAndValidate() {
    initLittleFs();
    printVersions();
    performSelfCheck();
}

/**
 * @brief Initializes LittleFS filesystem.
 */
void OtaManager::initLittleFs() {
    ESP_LOGI(TAG, "Inisialisasi LittleFS...");
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/lfs",
        .partition_label = "storage",
        .format_if_mount_failed = false,
        .dont_mount = false,
    };
    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Gagal inisialisasi LittleFS (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "LittleFS berhasil di-mount pada /lfs.");
    }
}

/**
 * @brief Prints firmware and data versions.
 */
void OtaManager::printVersions() {
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Versi Firmware Saat Ini: %s", CURRENT_FIRMWARE_VERSION);

    char ver_buf[20];
    FILE* f = fopen("/lfs/version.txt", "r");
    if (f != NULL) {
        if (fgets(ver_buf, sizeof(ver_buf), f) != NULL) {
            ver_buf[strcspn(ver_buf, "\r\n")] = 0;
            ESP_LOGI(TAG, "Versi Data (LFS) Terpasang: %s", ver_buf);
        }
        fclose(f);
    } else {
        ESP_LOGW(TAG, "File /lfs/version.txt tidak ditemukan.");
    }
    ESP_LOGI(TAG, "========================================");
}

/**
 * @brief Performs self-check and validates the application.
 */
void OtaManager::performSelfCheck() {
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
    esp_ota_mark_app_valid_cancel_rollback();
    ESP_LOGI(TAG, "Aplikasi ditandai sebagai valid (rollback dibatalkan).");
#else
    ESP_LOGW(TAG, "Rollback tidak diaktifkan. Validasi aplikasi dilewati.");
#endif
}

/**
 * @brief LED task for status indication.
 * @param pvParameter Pointer to OtaManager instance.
 */
void OtaManager::ledTaskRunner(void *pvParameter) {
    OtaManager* self = static_cast<OtaManager*>(pvParameter);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    bool ota_in_progress = false;

    while (1) {
        uint32_t notification_value;
        if (xTaskNotifyWait(0, ULONG_MAX, &notification_value, 0) == pdTRUE) {
            ota_in_progress = (bool)notification_value;
        }
        if (ota_in_progress) {
            gpio_set_level(LED_PIN, 1); vTaskDelay(pdMS_TO_TICKS(100));
            gpio_set_level(LED_PIN, 0); vTaskDelay(pdMS_TO_TICKS(100));
        } else {
            gpio_set_level(LED_PIN, 1); vTaskDelay(pdMS_TO_TICKS(1000));
            gpio_set_level(LED_PIN, 0); vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

/**
 * @brief OTA task for handling firmware updates.
 * @param pvParameter Pointer to OtaManager instance.
 */
void OtaManager::otaTaskRunner(void *pvParameter) {
    OtaManager* self = static_cast<OtaManager*>(pvParameter);
    int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(OTA_PORT);
    bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    listen(listen_sock, 1);
    ESP_LOGI(TAG, "OTA Task siap di port %d. Menunggu update...", OTA_PORT);

    char received_password[32] = {0};
    char ota_write_buf[OTA_BUF_SIZE];

    while (1) {
        struct sockaddr_in6 source_addr;
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TAG, "Gagal accept koneksi: errno %d", errno); continue;
        }

        ESP_LOGI(TAG, "Memulai proses OTA...");
        
        if (recv(sock, received_password, strlen(OTA_PASSWORD), MSG_WAITALL) != strlen(OTA_PASSWORD) || 
            strncmp(received_password, OTA_PASSWORD, strlen(OTA_PASSWORD)) != 0) {
            ESP_LOGE(TAG, "Kata sandi salah!");
            send(sock, "NO", 2, 0); close(sock); continue;
        }
        send(sock, "OK", 2, 0);
        ESP_LOGI(TAG, "Kata sandi diterima.");
        
        xTaskNotify(self->m_led_task_handle, 1, eSetValueWithOverwrite);

        const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
        esp_ota_handle_t update_handle = 0;
        esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_begin gagal: %s", esp_err_to_name(err));
            close(sock); xTaskNotify(self->m_led_task_handle, 0, eSetValueWithOverwrite); continue;
        }
        
        int data_read;
        while ((data_read = recv(sock, ota_write_buf, OTA_BUF_SIZE, 0)) > 0) {
            esp_ota_write(update_handle, (const void *)ota_write_buf, data_read);
        }
        
        if (data_read < 0 || esp_ota_end(update_handle) != ESP_OK || esp_ota_set_boot_partition(update_partition) != ESP_OK) {
            ESP_LOGE(TAG, "Update OTA Gagal!");
            close(sock); xTaskNotify(self->m_led_task_handle, 0, eSetValueWithOverwrite); continue;
        }
        close(sock);
        ESP_LOGI(TAG, "Update OTA berhasil. Restart dalam 3 detik...");
        vTaskDelay(pdMS_TO_TICKS(3000));
        esp_restart();
    }
}

} // namespace MyProject