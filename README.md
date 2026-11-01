# 🚀 ESP32 CubeSat – Real-Time Telemetry Dashboard

A compact ESP32-based telemetry system that collects, processes, and displays real-time sensor data using an IMU, barometric pressure sensor, and GPS receiver.

The project features a **128×64 OLED display** for live onboard monitoring and a Wi-Fi-powered web dashboard that streams sensor readings in JSON format for remote monitoring.

---

## 📌 Project Overview

This project demonstrates a compact CubeSat-inspired telemetry system using the ESP32 microcontroller.

It collects real-time data from multiple sensors, calculates orientation angles, tracks GPS coordinates, and displays the information on an OLED screen.

### Key Features

- Real-time acceleration and gyroscope readings.
- Roll and pitch angle calculation using a complementary filter.
- Atmospheric pressure and temperature monitoring.
- Humidity monitoring when using the BME280.
- GPS-based latitude, longitude, altitude, and speed.
- Satellite count and GPS fix status.
- 128×64 SSD1306 OLED live dashboard.
- Wi-Fi access point for wireless monitoring.
- JSON telemetry API.
- Mobile-friendly web dashboard.

---

## 🛠️ Hardware Requirements

| Component | Quantity |
|---|---:|
| ESP32 DevKit V1 | 1 |
| SSD1306 128×64 SPI OLED (7-pin) | 1 |
| MPU-6500 / MPU-9250 IMU | 1 |
| HW-611 BMP280 / BME280 Barometer | 1 |
| NEO-6M GPS Module | 1 |
| Jumper wires | As required |
| USB cable | 1 |
| Breadboard | 1 |

### Required Software

- Arduino IDE
- ESP32 Arduino Board Package
- Adafruit SSD1306 Library
- Adafruit GFX Library
- Adafruit BMP280 Library
- Adafruit BME280 Library
- TinyGPS++ Library

---

## 🔌 Circuit Connections

All sensors share a common 3.3V power supply and GND.

### 1. OLED Display – ESP32

Display: SSD1306 128×64 SPI (7-pin)

| OLED Pin | ESP32 Pin | Function |
|---|---|---|
| GND | GND | Ground |
| VCC | 3V3 | Power |
| D0 | GPIO 18 | SPI Clock |
| D1 | GPIO 23 | SPI Data |
| RES | GPIO 26 | Reset |
| DC | GPIO 27 | Data/Command |
| CS | GPIO 25 | Chip Select |

### 2. IMU Sensor – ESP32

Sensor: MPU-6500 / MPU-9250

| IMU Pin | ESP32 Pin | Function |
|---|---|---|
| VCC | 3V3 | Power |
| GND | GND | Ground |
| SDA | GPIO 21 | I2C Data |
| SCL | GPIO 22 | I2C Clock |
| AD0 | GND | I2C Address (0x68) |
| NCS | 3V3 | I2C Mode |
| FSYNC | GND | Disabled |

### 3. Barometric Sensor – ESP32

Sensor: HW-611 BMP280 / BME280

| Barometer Pin | ESP32 Pin | Function |
|---|---|---|
| VCC | 3V3 | Power |
| GND | GND | Ground |
| SDA | GPIO 21 | I2C Data |
| SCL | GPIO 22 | I2C Clock |
| CSB | 3V3 | I2C Mode |
| SDO | GND | I2C Address (0x76) |

### 4. GPS Module – ESP32

Module: NEO-6M GPS

| GPS Pin | ESP32 Pin | Function |
|---|---|---|
| VCC | 3V3 | Power* |
| GND | GND | Ground |
| TX | GPIO 32 | GPS UART Input |
| RX | Not Connected | Not Required |

*Verify the power requirements of your particular GPS breakout board before powering it. Some regulator-equipped boards require a different input voltage.*

---

## 🖼️ Wiring Diagram

The complete circuit connects four main modules to the ESP32:

1. OLED display through SPI.
2. MPU-6500/9250 through I2C.
3. BMP280/BME280 through shared I2C.
4. NEO-6M through UART.

**Important:** The IMU and barometer share GPIO 21 (SDA) and GPIO 22 (SCL).

---

## ⚙️ Assembly Instructions

### Step 1 – Prepare the ESP32

- Use an ESP32 DevKit V1.
- Disconnect USB power before wiring.
- Keep jumper wires short.
- Ensure all grounds are connected.

### Step 2 – Connect the OLED

Connect the seven OLED pins according to the wiring table.

### Step 3 – Connect the IMU

Connect the MPU-6500/9250 to the ESP32 using I2C.

Secure the IMU flat and keep its axis markings visible.

### Step 4 – Connect the Barometer

Connect the BMP280/BME280 to the shared I2C bus.

Keep the pressure sensor vent unobstructed.

### Step 5 – Connect the GPS

Connect the GPS TX output to ESP32 GPIO 32.

Position the antenna toward an open sky for better satellite reception.

The first GPS fix may take several minutes.

### Step 6 – Final Inspection

- Check all connections.
- Confirm correct voltage levels.
- Check for short circuits.
- Connect the ESP32 to a stable USB power supply.

**Estimated assembly time:** 20 minutes.

---

## 💻 Firmware Installation

### 1. Install Arduino IDE

Download and install Arduino IDE:

https://www.arduino.cc/en/software

### 2. Install ESP32 Board Support

Open Arduino IDE:

1. Navigate to Preferences.
2. Add the Espressif ESP32 board package URL.
3. Open Boards Manager.
4. Search for ESP32.
5. Install the ESP32 platform.

Official documentation:

https://docs.espressif.com/projects/arduino-esp32/

### 3. Install Required Libraries

Open Library Manager and install:

- Adafruit SSD1306
- Adafruit GFX Library
- Adafruit BMP280 Library
- Adafruit BME280 Library
- TinyGPSPlus

### 4. Add Project Files

Create a project folder:

```text
ESP32-CubeSat-Telemetry/
│
├── ESP32-CubeSat-Telemetry.ino
├── main.cpp
├── dashboard.h
└── README.md
```

The firmware uses `dashboard.h` to provide the HTML/CSS/JavaScript web interface through the `DASHBOARD` definition. This file must be created separately because its source was not included in the supplied firmware.

Copy the firmware into the project and ensure the required files are included.

### 5. Configure and Upload

- Select your ESP32 DevKit board.
- Select the correct COM/serial port.
- Set the upload speed as required.
- Compile and upload the firmware.
- Open Serial Monitor at 115200 baud.

---

## 📡 Wi-Fi Configuration

After successful initialization, the ESP32 creates its own Wi-Fi access point.

| Setting | Value |
|---|---|
| Wi-Fi Name (SSID) | CubeSat-Lab |
| Password | cubesat2026 |
| IP Address | 192.168.4.1 |
| Web Server Port | 80 |

### How to Access the Dashboard

1. Power on the ESP32.
2. Connect your phone or computer to `CubeSat-Lab`.
3. Enter the Wi-Fi password.
4. Open a browser.
5. Visit:

```text
http://192.168.4.1
```

The web interface displays live telemetry from the connected sensors.

---

## 📊 Telemetry API

The ESP32 hosts a JSON endpoint for remote monitoring.

**API Endpoint:**

```text
http://192.168.4.1/api/telemetry
```

### Example JSON Response

```json
{
  "uptime": 120,
  "imu": true,
  "baro": true,
  "fix": true,
  "ax": 0.02,
  "ay": 0.01,
  "az": 0.99,
  "gx": 0.12,
  "gy": -0.08,
  "gz": 0.04,
  "roll": 1.25,
  "pitch": -0.52,
  "temp": 28.5,
  "pressure": 1008.4,
  "humidity": 65.2,
  "lat": 12.971600,
  "lon": 77.594600,
  "alt": 920.5,
  "speed": 0.0,
  "sats": 8,
  "gpsChars": 12450
}
```

*Values are illustrative, not actual sensor measurements.*

### Available Telemetry Parameters

| Parameter | Description |
|---|---|
| uptime | System uptime in seconds |
| imu | IMU communication status |
| baro | Barometer status |
| fix | GPS fix status |
| ax, ay, az | Acceleration (g) |
| gx, gy, gz | Angular velocity (°/s) |
| roll | Roll angle (degrees) |
| pitch | Pitch angle (degrees) |
| temp | Temperature (°C) |
| pressure | Atmospheric pressure (hPa) |
| humidity | Relative humidity (%) |
| lat | GPS latitude |
| lon | GPS longitude |
| alt | GPS altitude (meters) |
| speed | GPS speed (km/h) |
| sats | Number of satellites |
| gpsChars | GPS characters processed |

Missing or unavailable floating-point readings are returned as JSON `null`.

---

## 🧠 Working Principle

### 1. Motion Detection

The MPU-6500/9250 measures acceleration and angular velocity along three axes.

The firmware uses accelerometer and gyroscope readings to calculate roll and pitch.

### 2. Orientation Calculation

A complementary filter combines accelerometer-based angles with gyroscope integration to provide more stable orientation estimates.

### 3. Environmental Monitoring

The BMP280/BME280 measures atmospheric pressure and temperature.

The BME280 additionally provides humidity measurements.

### 4. GPS Tracking

The NEO-6M receives satellite signals and sends NMEA data through UART.

TinyGPS++ parses the data to extract location, altitude, speed, and satellite information.

### 5. Wireless Telemetry

The ESP32 creates a Wi-Fi access point and hosts an HTTP server.

Sensor data is packaged into JSON and made available through the telemetry API.

---

## 🔧 Troubleshooting

| Problem | Possible Solution |
|---|---|
| OLED not displaying | Check SPI wiring and display controller |
| IMU not detected | Check I2C address 0x68 and wiring |
| Barometer not detected | Verify I2C address 0x76 |
| GPS has no location | Move antenna outdoors |
| Wi-Fi not visible | Restart ESP32 and check power |
| Dashboard not opening | Visit 192.168.4.1 |
| Incorrect sensor readings | Check wiring, sensor orientation, and calibration |

---

## ⚠️ Important Safety Notes

- Use 3.3V logic for ESP32 signal connections.
- Never apply 5V directly to ESP32 GPIO pins.
- Verify GPS breakout power requirements before connecting.
- Keep the GPS antenna away from electrical interference.
- Ensure the pressure sensor vent remains unobstructed.
- This is an educational tabletop prototype, not a flight-qualified or space-qualified telemetry system.

---

## 🚀 Future Improvements

- Add magnetometer-based heading estimation.
- Implement a 3D orientation visualization.
- Add SD card data logging.
- Introduce cloud-based telemetry monitoring.
- Implement MQTT communication.
- Add battery voltage and power monitoring.
- Develop a live GPS map interface.
- Improve sensor calibration and filtering.

---

## 🎯 Applications

- CubeSat telemetry demonstrations.
- Embedded systems development.
- IoT-based sensor monitoring.
- Robotics and motion tracking.
- GPS-based tracking systems.
- Aerospace engineering education.

---

## 👨‍💻 Project Information

**Project Name:** ESP32 CubeSat Real-Time Telemetry Dashboard

**Platform:** ESP32 DevKit V1

**Programming Language:** C++ (Arduino Framework)

**Communication:** SPI, I2C, UART, Wi-Fi

**Display:** SSD1306 128×64 OLED

**Telemetry Format:** JSON

**Estimated Assembly Time:** 20 Minutes

---

## 📜 License

This project is intended for educational and experimental purposes. A license can be added according to the project's distribution requirements.

---

**Built with ESP32, sensors, and a passion for aerospace technology. 🚀**
