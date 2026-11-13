ESP32 CubeSat Telemetry System

An ESP32-based CubeSat-inspired telemetry prototype featuring real-time sensor acquisition, onboard OLED monitoring, and a wireless web dashboard.

The system integrates an inertial measurement unit (IMU), barometric pressure sensor, and GPS receiver to collect and display motion, environmental, and location data.

Overview

This project explores embedded telemetry and remote monitoring through a compact hardware prototype.

An ESP32 collects sensor data, processes orientation measurements, and makes telemetry available through a local Wi-Fi network. Readings are presented on a 128 × 64 OLED display and a browser-based dashboard.

Project type: Embedded Systems / Aerospace-inspired Prototype
Controller: ESP32 DevKit V1
Firmware: C++ using Arduino Framework
Communication: SPI, I²C, UART, Wi-Fi
Telemetry format: JSON

Key Features

- Real-time accelerometer and gyroscope measurements.
- Roll and pitch estimation using a complementary filter.
- Barometric pressure and temperature monitoring.
- Optional humidity monitoring with BME280.
- GPS location, altitude, speed, and satellite information.
- SSD1306 OLED telemetry display.
- ESP32-hosted wireless access point.
- Browser-based live monitoring dashboard.
- JSON telemetry endpoint.
- Live sensor charts and session CSV export.

System Architecture

The system consists of four main sensor and display interfaces connected to the ESP32.

Module| Interface| Purpose
MPU-6500 / MPU-9250| I²C| Motion and orientation
BMP280 / BME280| I²C| Environmental measurements
NEO-6M GPS| UART| Position and speed
SSD1306 OLED| SPI| Onboard telemetry display

The ESP32 processes sensor measurements and serves the web dashboard over its own Wi-Fi network.

Hardware Requirements

Component| Quantity
ESP32 DevKit V1| 1
SSD1306 128 × 64 SPI OLED| 1
MPU-6500 / MPU-9250 IMU| 1
BMP280 / BME280 barometer| 1
NEO-6M GPS module| 1
Jumper wires| As required
Breadboard| 1
USB cable| 1

Pin Configuration

OLED — SPI

OLED pin| ESP32 GPIO
GND| GND
VCC| 3V3
D0 / SCLK| 18
D1 / MOSI| 23
RES| 26
DC| 27
CS| 25

IMU — I²C

IMU pin| ESP32 GPIO
VCC| 3V3
GND| GND
SDA| 21
SCL| 22
AD0| GND
NCS| 3V3

Barometer — I²C

Sensor pin| ESP32 GPIO
VCC| 3V3
GND| GND
SDA| 21
SCL| 22
CSB| 3V3
SDO| GND

GPS — UART

GPS pin| ESP32 GPIO
VCC| Verify module requirements
GND| GND
TX| 32
RX| Not connected

The IMU and barometer share the same I²C bus.

Software Requirements

- Arduino IDE
- ESP32 Arduino board package
- Adafruit SSD1306
- Adafruit GFX Library
- Adafruit BMP280 Library
- Adafruit BME280 Library
- TinyGPSPlus

Firmware Setup

1. Install Arduino IDE.
2. Install the ESP32 board package.
3. Install the required libraries.
4. Open the firmware sketch.
5. Configure the local Wi-Fi access point settings.
6. Select the ESP32 DevKit board and correct serial port.
7. Compile and upload the firmware.
8. Open Serial Monitor at 115200 baud.

Keep your actual Wi-Fi configuration in a local, untracked configuration file. Do not commit real credentials.

Accessing the Dashboard

After the ESP32 starts:

1. Connect your phone or computer to the configured CubeSat Wi-Fi network.
2. Open a browser.
3. Visit "http://192.168.4.1".

The browser dashboard provides live sensor readings, orientation visualization, telemetry history, and session CSV export.

The dashboard is hosted directly by the ESP32 and does not require an internet connection.

Telemetry API

The firmware exposes a JSON endpoint:

"http://192.168.4.1/api/telemetry"

Example response:

{
  "uptime": 120,
  "imu": true,
  "baro": true,
  "fix": true,
  "ax": 0.02,
  "ay": 0.01,
  "az": 0.99,
  "roll": 1.25,
  "pitch": -0.52,
  "temp": 28.5,
  "pressure": 1008.4,
  "lat": 12.9716,
  "lon": 77.5946,
  "alt": 920.5,
  "speed": 0.0,
  "sats": 8
}

Values are illustrative, not actual sensor measurements. Additional fields include gyroscope readings, humidity, and GPS processing information.

Operating Principle

1. Motion sensing

The IMU measures acceleration and angular velocity across three axes.

2. Orientation estimation

A complementary filter combines accelerometer-derived angles with gyroscope integration to estimate roll and pitch.

3. Environmental sensing

The BMP280 or BME280 measures atmospheric pressure and temperature. The BME280 additionally supports humidity measurement.

4. GPS tracking

The NEO-6M sends GPS data through UART. TinyGPSPlus parses location, altitude, speed, and satellite information.

5. Wireless telemetry

The ESP32 packages sensor readings into JSON and serves them through an HTTP endpoint for the web dashboard.

Safety and Limitations

- Use appropriate 3.3 V logic levels for ESP32 GPIO connections.
- Never apply 5 V directly to ESP32 GPIO pins.
- Verify the power requirements of individual sensor breakout boards.
- GPS reception may require an unobstructed view of the sky.
- Orientation estimates can be affected by acceleration and gyroscope drift.
- This is an educational tabletop prototype, not flight-qualified or space-qualified hardware.

Future Development

- Magnetometer integration and heading estimation.
- Improved sensor calibration and filtering.
- SD card telemetry logging.
- Battery voltage and power monitoring.
- Live GPS mapping.
- Cloud telemetry and MQTT integration.
- Improved 3D attitude visualization.

License

Distributed under the Mozilla Public License 2.0. See "LICENSE" (LICENSE) for details.

---

Developed by Dhyanraj Bangera
Embedded Systems · Electronics · Aerospace-inspired Prototyping
