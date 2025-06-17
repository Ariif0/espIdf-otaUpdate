import socket
import sys
import os
import time

ESP32_PORT = 3232
CHUNK_SIZE = 1024
OTA_PASSWORD = b"rahasia_sekali"  # Pastikan sama dengan OTA_PASSWORD di main.c

if len(sys.argv) != 3:
    print("Penggunaan: python ota_script.py <IP_ESP32> <PATH_KE_FIRMWARE.BIN>")
    sys.exit(1)

esp32_ip = sys.argv[1]
firmware_path = sys.argv[2]

if not os.path.exists(firmware_path):
    print(f"Error: File firmware tidak ditemukan di '{firmware_path}'")
    sys.exit(1)

file_size = os.path.getsize(firmware_path)
print(f"Target IP: {esp32_ip}:{ESP32_PORT}")
print(f"File Firmware: {firmware_path} ({file_size} bytes)")

try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(10)
        print("Menyambung ke ESP32...")
        s.connect((esp32_ip, ESP32_PORT))
        print("Tersambung! Mengirim kata sandi...")

        # Kirim kata sandi
        s.sendall(OTA_PASSWORD)
        response = s.recv(2).decode()
        if response != "OK":
            print("Kata sandi salah! Koneksi ditutup oleh ESP32.")
            sys.exit(1)
        print("Kata sandi diterima. Mulai mengirim file...")

        # Kirim data firmware
        with open(firmware_path, 'rb') as f:
            bytes_sent = 0
            while True:
                chunk = f.read(CHUNK_SIZE)
                if not chunk:
                    break
                s.sendall(chunk)
                bytes_sent += len(chunk)
                progress = (bytes_sent / file_size) * 100
                print(f"Mengirim... {bytes_sent}/{file_size} bytes ({progress:.2f}%)", end='\r')
                time.sleep(0.01)

        if bytes_sent != file_size:
            print(f"\nError: Hanya {bytes_sent} bytes terkirim, seharusnya {file_size} bytes!")
            sys.exit(1)

        print("\n\nPengiriman file selesai. Menunggu ESP32 restart...")
        time.sleep(2)
        print("Update OTA Berhasil!")
except Exception as e:
    print(f"\nTerjadi error: {e}")