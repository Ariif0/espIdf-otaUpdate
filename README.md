# **ESP32 OTA Firmware with C++ & ESP-IDF**

**A robust, object-oriented firmware for ESP32 enabling Over-The-Air (OTA) updates via a secure TCP socket.**

<p align="center">
  <img alt="Language" src="https://img.shields.io/badge/Language-C%2B%2B-blue.svg?style=for-the-badge">
  <img alt="Framework" src="https://img.shields.io/badge/Framework-ESP--IDF-red.svg?style=for-the-badge">
  <img alt="OS" src="https://img.shields.io/badge/OS-FreeRTOS-green.svg?style=for-the-badge">
</p>

-----

### 📖 **Description**

This project is an ESP32 firmware implementation enabling Over-The-Air (OTA) updates over Wi-Fi. Built with the ESP-IDF framework and FreeRTOS, it uses an Object-Oriented Programming (OOP) approach for a clean, modular, and maintainable architecture. The code includes Doxygen-ready documentation and memory optimizations (e.g., static buffers, reduced task stack sizes).

### 🚀 **Key Features**

  * **Wi-Fi OTA Updates:** Wirelessly update firmware via a password-protected TCP socket.
  * **LittleFS File System:** Stores metadata (e.g., version info) in a dedicated storage partition.
  * **OOP Architecture:** Logic is cleanly separated into `Application`, `WifiManager`, and `OtaManager` classes.
  * **LED Status Indicator:** Provides intuitive visual feedback with slow blinks (1s) when idle and fast blinks (100ms) during an OTA process.
  * **Secure Configuration:** Isolates sensitive credentials into an untracked `secrets.h` file.

### 🛠️ **Tech Stack & Requirements**

#### Hardware

  * ESP32 Dev Module (or a similar board).

#### Software

  * Visual Studio Code with the **PlatformIO** extension.
  * **ESP-IDF** v4.4 or newer (managed by PlatformIO).
  * **Python 3.x** for the OTA update script.
  * **Git** for version control.

-----

### 📋 **Getting Started Guide**

Follow these steps to set up, build, and run the project on your ESP32.

#### 1\. **Clone & Configure the Project**

First, clone the repository and configure your local environment.

```bash
git clone https://github.com/Ariif0/espIdf-otaUpdate.git
cd espIdf-otaUpdate
```

Next, create a `secrets.h` file inside the `include/` directory for your Wi-Fi credentials. This file is ignored by Git to keep your secrets safe.

\<details\>
\<summary\>\<b\>Click to see an example of \<code\>include/secrets.h\</code\>\</b\>\</summary\>

```cpp
#pragma once

/**
 * @file secrets.h
 * @brief Sensitive configuration for Wi-Fi credentials.
 */

// --- Wi-Fi Credentials ---
#define WIFI_SSID "YOUR_WIFI_SSID"      ///< Replace with your Wi-Fi SSID
#define WIFI_PASS "YOUR_WIFI_PASSWORD"  ///< Replace with your Wi-Fi password
```

\</details\>
\<br\>

> **Note:**
>
>   - Adjust `OTA_PORT`, `OTA_PASSWORD`, or `LED_PIN` in `include/config.h` if needed.
>   - Ensure the `board` in `platformio.ini` matches your ESP32 variant (e.g., `esp32doit-devkit-v1`).

#### 2\. **Initial Upload (via Cable)**

Flash the initial firmware and filesystem using a USB connection.

1.  **Upload Firmware:**
    ```bash
    pio run --target upload
    ```
2.  **Upload LittleFS Filesystem:**
    ```bash
    pio run --target uploadfs
    ```

### 💨 **OTA Update Process**

Perform wireless updates after the initial setup.

1.  **Monitor Device & Get IP Address:**
    Open the PlatformIO Serial Monitor to view device logs and find its assigned IP address.
    ```bash
    pio device monitor
    ```
2.  **Prepare New Firmware:**
    Make changes to your code (e.g., update `CURRENT_FIRMWARE_VERSION` in `config.h`), then compile to create a new binary file. **Do not upload via cable.**
    ```bash
    pio run
    ```
3.  **Send Update via Python Script:**
    Use the provided `ota_script.py` to send the new `firmware.bin` file.
    ```bash
    python ota_script.py <ESP32_IP_ADDRESS> .pio/build/esp32doit-devkit-v1/firmware.bin
    ```
    > **Important:** Replace `<ESP32_IP_ADDRESS>` with your device's IP and ensure the path to `firmware.bin` is correct for your board.

-----

### 🔍 **Development & Testing Tips**

  * **Authentication:** Test the OTA process with an incorrect password to verify that the security mechanism correctly rejects the connection.
  * **Error Handling:** Try sending an invalid or corrupted firmware file to ensure the device handles the error gracefully without crashing.
  * **Memory Optimization:** To check a task's stack usage, add the following log inside its main loop:
    ```cpp
    ESP_LOGI("TASK_NAME", "Stack High Water Mark: %d bytes", uxTaskGetStackHighWaterMark(NULL) * sizeof(StackType_t));
    ```
    > This helps you fine-tune stack sizes and save RAM. A remaining margin of \>200 bytes is generally recommended.

### 👤 **Author**

Developed and maintained by **Muhamad Arif Hidayat**.

### 📜 **License**

This project is licensed under the MIT License. See the `LICENSE` file for details.
