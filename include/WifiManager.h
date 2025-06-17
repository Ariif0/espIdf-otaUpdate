#pragma once
#include "sdk_compat.h"

namespace MyProject {

/**
 * @class WifiManager
 * @brief Manages Wi-Fi connectivity for the ESP32.
 *
 * This class handles Wi-Fi initialization, connection, and event handling using
 * ESP-IDF and FreeRTOS. It provides methods to connect to a Wi-Fi network and
 * check connection status.
 */
class WifiManager {
public:
    /**
     * @brief Constructor for WifiManager.
     *
     * Initializes the event group for Wi-Fi events.
     */
    WifiManager();

    /**
     * @brief Destructor for WifiManager.
     *
     * Cleans up the event group resources.
     */
    ~WifiManager();

    /**
     * @brief Connects to the configured Wi-Fi network.
     * @return esp_err_t ESP_OK on success, ESP_FAIL on failure.
     */
    esp_err_t connect();

    /**
     * @brief Checks if Wi-Fi is connected.
     * @return bool True if connected, false otherwise.
     */
    bool isConnected();

private:
    EventGroupHandle_t m_wifi_event_group; ///< FreeRTOS event group for Wi-Fi events
    int m_retry_num;                       ///< Number of reconnect attempts
    bool m_is_connected;                   ///< Wi-Fi connection status

    /**
     * @brief Static event handler for Wi-Fi and IP events.
     * @param arg Pointer to WifiManager instance.
     * @param event_base Event base (WIFI_EVENT or IP_EVENT).
     * @param event_id Event ID.
     * @param event_data Event data.
     */
    static void eventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    /**
     * @brief Processes Wi-Fi and IP events.
     * @param event_base Event base (WIFI_EVENT or IP_EVENT).
     * @param event_id Event ID.
     * @param event_data Event data.
     */
    void onWifiEvent(esp_event_base_t event_base, int32_t event_id, void* event_data);
};

} // namespace MyProject