#pragma once
#include "sdk_compat.h"
#include "WifiManager.h"
#include "OtaManager.h"

namespace MyProject {

/**
 * @class Application
 * @brief Main application class for coordinating system components.
 *
 * This class initializes Non-Volatile Storage (NVS), Wi-Fi, and OTA services,
 * and runs the main application logic.
 */
class Application {
public:
    /**
     * @brief Constructor for Application.
     *
     * Initializes Wi-Fi and OTA managers.
     */
    Application();

    /**
     * @brief Runs the main application logic.
     */
    void run();

private:
    WifiManager m_wifi; ///< Wi-Fi manager instance
    OtaManager m_ota;   ///< OTA manager instance

    /**
     * @brief Initializes Non-Volatile Storage (NVS).
     */
    void initializeNVS();
};

} // namespace MyProject