#pragma once

/**
 * @file sdk_compat.h
 * @brief Compatibility header for ESP-IDF, FreeRTOS, and C++ standard library includes.
 *
 * This header consolidates necessary includes for the project, ensuring compatibility
 * between C and C++ codebases.
 */

#ifdef __cplusplus
extern "C" {
#endif
// --- FreeRTOS Core ---
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"

// --- ESP-IDF Core ---
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

// --- ESP-IDF Drivers & Libs ---
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_ota_ops.h"
#include "esp_littlefs.h"

// --- LwIP (for Sockets) ---
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#ifdef __cplusplus
}
#endif

// --- C++ Standard Library ---
#include <string>
#include <vector>
#include <functional>

namespace MyProject {
  // All project-specific classes will reside in this namespace
}