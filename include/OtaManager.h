#pragma once
#include "sdk_compat.h"

namespace MyProject {

/**
 * @class OtaManager
 * @brief Manages Over-The-Air (OTA) firmware updates and system validation.
 *
 * This class handles OTA updates via a TCP socket, LittleFS initialization,
 * and LED status indication. It also performs firmware validation and version checking.
 */
class OtaManager {
public:
    /**
     * @brief Constructor for OtaManager.
     *
     * Initializes the OTA task handle.
     */
    OtaManager();

    /**
     * @brief Starts the OTA and LED tasks.
     */
    void start();

    /**
     * @brief Initializes LittleFS and validates the system.
     */
    void initializeAndValidate();

private:
    TaskHandle_t m_led_task_handle; ///< FreeRTOS task handle for LED task

    /**
     * @brief Initializes LittleFS filesystem.
     */
    void initLittleFs();

    /**
     * @brief Prints firmware and data versions.
     */
    void printVersions();

    /**
     * @brief Performs self-check and validates the application.
     */
    void performSelfCheck();

    /**
     * @brief LED task for status indication.
     * @param pvParameter Pointer to OtaManager instance.
     */
    static void ledTaskRunner(void* pvParameter);

    /**
     * @brief OTA task for handling firmware updates.
     * @param pvParameter Pointer to OtaManager instance.
     */
    static void otaTaskRunner(void* pvParameter);
};

} // namespace MyProject