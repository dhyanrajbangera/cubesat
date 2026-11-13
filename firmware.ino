CubeSat Telemetry Firmware

Embedded firmware for the ESP32-based CubeSat Telemetry System.

This firmware collects data from multiple sensors, processes telemetry, displays measurements on an OLED screen, and hosts a wireless web dashboard for real-time monitoring.

Overview

The firmware runs on an ESP32 DevKit V1 and integrates:

- MPU-6500 / MPU-9250 inertial measurement unit
- BMP280 / BME280 environmental sensor
- NEO-6M GPS receiver
- SSD1306 128 × 64 OLED display
- ESP32 Wi-Fi access point and HTTP server

The firmware is developed using C++ and the Arduino framework.

Directory Structure

firmware/
│
├── README.md
├── CubeSatTelemetry.ino
├── dashboard.h
└── config.example.h

File| Description
"CubeSatTelemetry.ino"| Main ESP32 firmware and application logic
"dashboard.h"| Embedded web dashboard interface
"config.example.h"| Example configuration template
"README.md"| Firmware documentation

Firmware Features

Sensor Acquisition

- Reads accelerometer and gyroscope measurements.
- Collects temperature and atmospheric pressure.
- Reads humidity when using a BME280.
- Receives GPS location and satellite information.

Orientation Processing

- Calculates roll and pitch.
- Uses a complementary filter to combine accelerometer and gyroscope data.
- Provides continuous orientation estimates.

OLED Display

- Displays sensor readings.
- Shows orientation information.
- Provides onboard telemetry monitoring.

Wireless Telemetry

- Creates a local ESP32 Wi-Fi access point.
- Hosts a browser-based monitoring dashboard.
- Exposes sensor readings through a JSON endpoint.
- Supports live telemetry visualization.

Hardware Interfaces

Module| Communication| ESP32 Pins
IMU| I²C| SDA 21, SCL 22
BMP280 / BME280| I²C| SDA 21, SCL 22
OLED| SPI| SCLK 18, MOSI 23, CS 25, DC 27, RES 26
GPS| UART| RX 32

The IMU and barometer share the I²C bus.

Verify the actual sensor breakout pin requirements and wiring before powering the system.

Required Libraries

Install the following libraries through the Arduino IDE Library Manager:

- Adafruit GFX Library
- Adafruit SSD1306
- Adafruit BMP280 Library
- Adafruit BME280 Library
- TinyGPSPlus

Also install the ESP32 board package using Arduino IDE Boards Manager.

Installation and Setup

1. Requirements

- ESP32 DevKit V1
- Arduino IDE
- USB data cable
- Required sensor modules
- Required Arduino libraries

2. Configure the Firmware

1. Open the firmware project.
2. Review the Wi-Fi configuration.
3. Configure the access point SSID and password.
4. Check the sensor configuration and I²C addresses.
5. Confirm the GPS UART configuration.

Keep real Wi-Fi credentials in a local configuration file that is excluded from Git.

3. Upload

1. Connect the ESP32 to your computer.
2. Open "CubeSatTelemetry.ino".
3. Select ESP32 Dev Module in Arduino IDE.
4. Select the correct COM port.
5. Compile the firmware.
6. Upload the program to the ESP32.

4. Monitor

Open Arduino IDE Serial Monitor at:

"115200 baud"

Check the startup messages and sensor initialization status.

Accessing the Telemetry Dashboard

After successful firmware initialization:

1. Connect your phone or computer to the ESP32 access point.
2. Open a web browser.
3. Navigate to:

"http://192.168.4.1"

The dashboard provides access to the live telemetry interface.

JSON Telemetry Endpoint

"http://192.168.4.1/api/telemetry"

The endpoint provides structured sensor information in JSON format.

Development Notes

- Sensor readings depend on successful hardware initialization.
- GPS measurements require suitable satellite reception.
- Sensor calibration may be necessary for accurate orientation estimates.
- Wi-Fi credentials should never be committed to the public repository.
- Pin assignments must match the actual hardware connections.

Troubleshooting

Problem| Suggested Check
IMU not detected| Check I²C wiring and address
Barometer unavailable| Verify sensor type and I²C address
GPS has no fix| Check UART connections and outdoor reception
OLED not displaying| Verify SPI connections and display configuration
Dashboard unavailable| Check Wi-Fi initialization and ESP32 connection
Incorrect sensor values| Check calibration and power supply

Safety Notice

This firmware is intended for an educational CubeSat-inspired prototype. It is not certified for actual satellite deployment or mission-critical aerospace applications.

License

This firmware is distributed under the Mozilla Public License 2.0. See the repository's "LICENSE" file for details.

---

Developed by Dhyanraj Bangera
Embedded Systems | ESP32 | Sensor Integration | Wireless Telemetry
