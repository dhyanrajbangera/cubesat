# ESP32 CubeSat Telemetry System

[![Build firmware](../../actions/workflows/build.yml/badge.svg)](../../actions/workflows/build.yml)
![Platform: ESP32](https://img.shields.io/badge/platform-ESP32-blue)
![Framework: Arduino](https://img.shields.io/badge/framework-Arduino-00979D)
![Built with PlatformIO](https://img.shields.io/badge/built%20with-PlatformIO-orange)
![License: MPL-2.0](https://img.shields.io/badge/license-MPL--2.0-brightgreen)

An ESP32-based CubeSat flight-computer prototype that reads motion, environment and GPS sensors, shows live data on an OLED, and serves a **Mission Control web dashboard** over Wi-Fi. No cloud account or internet connection is needed.

<!-- Add a photo of the build and a dashboard screenshot here:
![Dashboard](docs/images/dashboard.png) -->

## Features

- **Attitude estimation:** roll and pitch from an MPU-series IMU using a complementary filter (gyro + accelerometer).
- **Environment sensing:** temperature, pressure and humidity from a BMP280/BME280 (auto-detected), with a DHT11 as fallback.
- **GPS:** position, altitude, speed and satellite count via TinyGPS++, with fix-age validation.
- **Live OLED display:** animated 3D cube boot screen, then live readings and the dashboard address.
- **Mission Control dashboard:** attitude/3D cube view, roll/pitch and temperature history charts, GPS map and trail, system status and mission summary.
- **Wi-Fi setup page:** join your router from a phone; the credentials are stored on the ESP32. The `CubeSat-Lab` access point stays available as a backup.
- **JSON API:** `/api/telemetry` and `/api/history` (last 15 minutes, 1 sample per second).
- **Sensor hot-plug:** missing sensors are re-detected every 10 seconds.
- **Audio cues:** non-blocking buzzer melodies for boot, Wi-Fi connected and GPS fix.

## Hardware

| Part | Purpose |
|------|---------|
| ESP32 dev board (`esp32dev`) | Main computer |
| SSD1306 128×64 OLED (SPI) | Local display |
| MPU-6500/9250/9255 IMU (I²C) | Accelerometer and gyro |
| BMP280 or BME280 (I²C) | Pressure, temperature (and humidity on BME280) |
| DHT11 | Backup temperature and humidity |
| NMEA GPS module, 9600 baud | Position |
| Passive buzzer | Sound alerts |

Pin mapping and wiring notes are in [docs/hardware.md](docs/hardware.md).

## Getting started

1. Install [PlatformIO](https://platformio.org/install) (VS Code extension or CLI).
2. Clone and build:
   ```bash
   git clone https://github.com/dhyanrajbangera/cubesat.git
   cd cubesat
   pio run
   ```
3. Connect the ESP32 by USB and upload, then open the serial monitor:
   ```bash
   pio run -t upload
   pio device monitor
   ```
4. Connect your phone or laptop to the Wi-Fi network **CubeSat-Lab** and open **http://192.168.4.1**.

> The default access-point password is defined in `src/main.cpp`. **Change it before deploying** (see [Security](#security)).

### Joining your home or lab router

Open `http://192.168.4.1/wifi`, enter your 2.4 GHz network name and password, and save. The OLED then shows the new IP address; open it from a device on the same network.

## Repository layout

```
├── src/main.cpp          Firmware: sensors, filter, web server, OLED, sound
├── include/dashboard.h   Mission Control web UI (embedded in flash)
├── include/wifi_setup.h  Wi-Fi setup web page
├── docs/                 Hardware pinout and API reference
├── platformio.ini        Board, libraries, build settings
└── .github/workflows/    CI: builds the firmware on every push
```

## API

| Endpoint | Description |
|----------|-------------|
| `GET /` | Mission Control dashboard |
| `GET /wifi` | Wi-Fi setup page |
| `GET /api/telemetry` | Current readings as JSON |
| `GET /api/history` | Up to 900 one-second samples (roll, pitch, temp, pressure) |
| `GET /api/network` | Wi-Fi connection status |

Full field descriptions: [docs/api.md](docs/api.md).

## Security

This project is intended for **trusted local networks**.

- The dashboard uses plain HTTP with no login. Anyone on the same network, or with the backup access-point password, can view it.
- Do not expose the device to the internet or enable port forwarding.
- Change the default `CubeSat-Lab` password before sharing the device.

## Roadmap

- [ ] SD-card or flash data logging
- [ ] Magnetometer heading
- [ ] Radio downlink (LoRa)
- [ ] Unit tests for the filter and JSON output

## Contributing

Issues and pull requests are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

Released under the [Mozilla Public License 2.0](LICENSE).

## Author

Made by **Dhyan**.
