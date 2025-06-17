/**
 * @file main.cpp
 * @brief Entry point for the ESP32 application.
 *
 * This file defines the main application instance and the entry point function
 * required by ESP-IDF to start the application.
 */

#include "Application.h"

namespace MyProject {
/**
 * @brief Global application instance.
 *
 * This instance is used to run the main application logic.
 */
Application app;
}

/**
 * @brief Main entry point for the ESP32 application.
 *
 * This function is called by the ESP-IDF framework to start the application.
 * It invokes the run method of the global Application instance.
 */
extern "C" void app_main(void) {
    MyProject::app.run();
}