# Hardware

Pins below are taken from `src/main.cpp`. Verify them against your own wiring.

## Pin map (ESP32)

| Function | Pin(s) |
|----------|--------|
| I²C SDA / SCL (IMU, BMP/BME280) | GPIO 21 / GPIO 22 (100 kHz) |
| OLED SPI SCK / MOSI | GPIO 18 / GPIO 23 |
| OLED DC | GPIO 27 |
| OLED RESET | GPIO 26 |
| OLED CS | GPIO 25 |
| GPS TX → ESP32 RX (Serial2, 9600 baud) | GPIO 32 |
| DHT11 data | GPIO 4 |
| Buzzer | GPIO 13 |

## I²C addresses

| Device | Address | Detection |
|--------|---------|-----------|
| IMU (MPU-6500/9250/9255) | 0x68 or 0x69 | WHO_AM_I = 0x70, 0x71 or 0x73 |
| BME280 | 0x76 or 0x77 | chip ID 0x60 |
| BMP280 | 0x76 or 0x77 | chip ID 0x58 |

## Notes

- Power the OLED, sensors and GPS from 3.3 V unless your module states otherwise.
- The GPS TX line goes to the ESP32; only the receive pin is configured.
- The Wi-Fi radio works on 2.4 GHz only.
