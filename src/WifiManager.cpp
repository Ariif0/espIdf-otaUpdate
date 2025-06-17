/**
 * @file WifiManager.cpp
 * @brief Implementation of the WifiManager class for Wi-Fi connectivity.
 *
 * This file contains the implementation of the WifiManager class, which handles
 * Wi-Fi initialization, connection, and event processing using ESP-IDF and FreeRTOS.
 */

#include "WifiManager.h"
#include "config.h"

namespace MyProject {

static const char* TAG = "WifiManager";
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

/**
 * @brief Constructor for WifiManager.
 *
 * Initializes the event group for Wi-Fi events and sets initial state.
 */
WifiManager::WifiManager() : m_retry_num(0), m_is_connected(false) {
    m_wifi_event_group = xEventGroupCreate();
}

/**
 * @brief Destructor for WifiManager.
 *
 * Cleans up the event group resources.
 */
WifiManager::~WifiManager() {
    vEventGroupDelete(m_wifi_event_group);
}

/**
 * @brief Checks if Wi-Fi is connected.
 * @return bool True if connected, false otherwise.
 */
bool WifiManager::isConnected() {
    return m_is_connected;
}

/**
 * @brief Static event handler for Wi-Fi and IP events.
 * @param arg Pointer to WifiManager instance.
 * @param event_base Event base (WIFI_EVENT or IP_EVENT).
 * @param event_id Event ID.
 * @param event_data Event data.
 */
void WifiManager::eventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    WifiManager* self = static_cast<WifiManager*>(arg);
    self->onWifiEvent(event_base, event_id, event_data);
}

/**
 * @brief Processes Wi-Fi and IP events.
 * @param event_base Event base (WIFI_EVENT or IP_EVENT).
 * @param event_id Event ID.
 * @param event_data Event data.
 */
void WifiManager::onWifiEvent(esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        m_is_connected = false;
        if (m_retry_num < WIFI_MAX_RECONNECT_ATTEMPTS) {
            esp_wifi_connect();
            m_retry_num++;
            ESP_LOGI(TAG, "Mencoba menyambung ulang ke AP...");
        } else {
            xEventGroupSetBits(m_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGE(TAG, "Gagal menyambung ke AP.");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Mendapat IP: " IPSTR, IP2STR(&event->ip_info.ip));
        m_retry_num = 0;
        m_is_connected = true;
        xEventGroupSetBits(m_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/**
 * @brief Connects to the configured Wi-Fi network.
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t WifiManager::connect() {
    ESP_LOGI(TAG, "Memulai Wi-Fi...");
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler, this));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler, this));

    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASS);
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Menunggu koneksi Wi-Fi...");

    EventBits_t bits = xEventGroupWaitBits(m_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS));

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Wi-Fi Terhubung!");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Gagal terhubung ke Wi-Fi dalam %d ms.", WIFI_CONNECT_TIMEOUT_MS);
        return ESP_FAIL;
    }
}

} // namespace MyProject