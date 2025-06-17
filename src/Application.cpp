/**
 * @file Application.cpp
 * @brief Implementation of the Application class for coordinating system components.
 *
 * This file contains the implementation of the Application class, which initializes
 * Non-Volatile Storage (NVS), Wi-Fi, and OTA services, and runs the main application logic.
 */

#include "Application.h"
#include "config.h"

namespace MyProject {

static const char* TAG = "Application";

/**
 * @brief Constructor for Application.
 *
 * Initializes Wi-Fi and OTA managers.
 */
Application::Application() {}

/**
 * @brief Initializes Non-Volatile Storage (NVS).
 */
void Application::initializeNVS() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS diinisialisasi.");
}

/**
 * @brief Runs the main application logic.
 */
void Application::run() {
    initializeNVS();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    m_ota.initializeAndValidate();

    if (m_wifi.connect() == ESP_OK) {
        ESP_LOGI(TAG, "Wi-Fi OK. Memulai layanan utama.");
        m_ota.start();
    } else {
        ESP_LOGE(TAG, "Koneksi Wi-Fi Gagal. Restart dalam 10 detik.");
        gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
        for(int i=0; i<20; i++) {
            gpio_set_level(LED_PIN, 1); vTaskDelay(pdMS_TO_TICKS(250));
            gpio_set_level(LED_PIN, 0); vTaskDelay(pdMS_TO_TICKS(250));
        }
        esp_restart();
    }

    ESP_LOGI(TAG, "Setup selesai. Task-task berjalan di background.");
}

} // namespace MyProject