# **ESP32 OTA Firmware Project with ESP-IDF*

## **Description**

This project is an ESP32 firmware implementation that supports Over-The-Air (OTA) updates over Wi-Fi. The code is built using the ESP-IDF framework and FreeRTOS, applying an Object-Oriented Programming (OOP) approach in C++ to create a clean, modular, and maintainable architecture.

## **Key Features**

  - **Wi-Fi OTA Updates:** Wirelessly update the device firmware through a password-protected TCP socket.
  - **LittleFS File System:** Utilizes a `storage` partition with LittleFS to store metadata, such as version information.
  - **Object-Oriented Architecture:** Program logic is neatly separated into distinct classes (`Application`, `WifiManager`, `OtaManager`).
  - **LED Status Indicator:** Provides intuitive visual feedback (slow blink when idle, fast blink during an OTA process).

## **Requirements**

### **Hardware**

  - ESP32 Dev Module (or a similar board).

### **Software**

  - Visual Studio Code with the **PlatformIO** extension.
  - **ESP-IDF** framework v4.4 or newer (managed by PlatformIO).
  - **Python 3.x** to run the OTA update script.
  - **Git** for version control.

## **Getting Started Guide**

Follow these steps to configure and run this project on your device.

### **1. Project Configuration**

After cloning this repository, perform the following initial setup:

**a. Set Up Wi-Fi Credentials**

Create a new file at `include/secrets.h`. This file is intentionally untracked by Git to keep your credentials secure.

```cpp
#pragma once

/**
 * @file secrets.h
 * @brief Sensitive configuration for Wi-Fi credentials.
 *
 * This file defines sensitive configuration constants for Wi-Fi connectivity.
 * It is separated from other configurations to enhance security and maintainability.
 */

// --- Wi-Fi Credentials ---
#define WIFI_SSID "YOUR_WIFI_SSID"      ///< Replace with your Wi-Fi SSID
#define WIFI_PASS "YOUR_WIFI_PASSWORD"  ///< Replace with your Wi-Fi password

```

**b. Other Configurations**

  - Adjust parameters such as `OTA_PORT`, `OTA_PASSWORD`, and `LED_PIN` in the `include/config.h` file if needed.
  - Ensure the `board` in `platformio.ini` matches the ESP32 board you are using (e.g., `esp32doit-devkit-v1`).

### **2. Initial Upload (via Cable)**

Perform the first upload using a USB connection to flash the initial firmware and filesystem.

1.  **Upload Main Firmware:**
    ```bash
    pio run --target upload
    ```
2.  **Upload LittleFS Filesystem:**
    ```bash
    pio run --target uploadfs
    ```

## **OTA Update Process**

Once the initial program is running, you can perform subsequent updates wirelessly.

1.  **Monitor Device & Get IP Address:**
    Open the Serial Monitor to view device logs and find its assigned IP address.

    ```bash
    pio device monitor
    ```

2.  **Prepare New Firmware:**
    Make changes to your code (e.g., update `CURRENT_FIRMWARE_VERSION` in `config.h`), then compile to create a new binary file. **Do not upload via cable.**

    ```bash
    pio run
    ```

3.  **Send Update with Python Script:**
    Use the `ota_script.py` to send the newly built `firmware.bin` file.

    ```bash
    python ota_script.py <ESP32_IP_ADDRESS> .pio/build/esp32doit-devkit-v1/firmware.bin
    ```

    > **Note:** Replace `<ESP32_IP_ADDRESS>` with your device's IP and ensure the path to `firmware.bin` is correct for your board.

4.  **Verify OTA Result:**

      - **Observe the LED:** The LED will blink rapidly (100ms) during the OTA process. If successful, the device will restart. If it fails, it will return to a slow blink (1s).
      - **Check the Logs:** View the Serial Monitor for status messages like `"OTA update successful. Rebooting in 3 seconds..."` or `"OTA Update Failed!"`.

## **Further Development & Testing**

Here are some ideas for further testing and development:

  - Test the authentication mechanism by attempting an OTA update with an incorrect password.
  - Test the error handling by trying to send an invalid or corrupted firmware file.
  - To optimize memory, check a task's stack usage by adding the following log inside its loop:
    ```cpp
    ESP_LOGI("TASK_NAME", "Stack High Water Mark: %d bytes", uxTaskGetStackHighWaterMark(NULL) * sizeof(StackType_t));
    ```

> **Note:** This code provides a solid and functional foundation for OTA updates on the ESP-IDF via a TCP socket. Feel free to use and expand upon it for your needs.

## **Author**

Developed and maintained by **Muhamad Arif Hidayat**.