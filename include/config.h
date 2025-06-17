#pragma once
#include "secrets.h"

/**
 * @file config.h
 * @brief Configuration constants for Wi-Fi, OTA, and hardware settings.
 *
 * This file defines non-sensitive constants used throughout the project, such as
 * Wi-Fi reconnection settings, OTA configurations, and hardware pin assignments.
 * Sensitive Wi-Fi credentials are defined in secrets.h.
 */

namespace MyProject {
// --- Wi-Fi Configuration ---
#define WIFI_MAX_RECONNECT_ATTEMPTS 5  ///< Maximum Wi-Fi reconnect attempts
#define WIFI_CONNECT_TIMEOUT_MS 20000  ///< Wi-Fi connection timeout (ms)

// --- OTA Configuration ---
#define OTA_PORT       3232            ///< OTA server port
#define OTA_BUF_SIZE   1024            ///< OTA buffer size
#define OTA_PASSWORD   "rahasia_sekali"///< OTA authentication password
#define CURRENT_FIRMWARE_VERSION "v2.0-oop" ///< Current firmware version

// --- Hardware Pin Configuration ---
#define LED_PIN        (gpio_num_t)2   ///< LED GPIO pin
}