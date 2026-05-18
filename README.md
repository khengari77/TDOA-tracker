# TDOA Tracker — LoRa-Based Real-Time Localization System

![Build](https://github.com/khengari77/TDOA-tracker/workflows/Build/badge.svg?branch=development)

Gateway firmware for a **LoRa-based TDOA (Time Difference of Arrival) localization system** designed to track pilgrims during Hajj in Makkah. This is the code running on **TTGO T-Beam gateways** — it receives LoRa packets from end-node badges, records timing and RSSI, reads GPS position, and forwards everything to the cloud via MQTT.

## System Architecture

```
[ Pilgrim Badge (ESP32 + LoRa + OLED) ]
        │
        ▼ LoRa (433 MHz)
[ T-Beam Gateway (ESP32-S3 + LoRa + GPS + PMU) ]
        │
        ▼ MQTT over WiFi
[ HiveMQ Cloud Broker ]
        │
        ▼
[ Dashboard (Python TDOA + Kalman + Svelte/Leaflet) ]
```

- **End Nodes** — LILYGO LoRa V1.4 boards worn by pilgrims; transmit periodic LoRa packets with a unique ID
- **Gateways** — TTGO T-Beam boards deployed around the pilgrimage area; receive LoRa packets, record TOA + RSSI, attach GPS position, and publish to MQTT
- **Dashboard** — Server-side Python app runs TDOA multilateration and a Kalman filter to estimate positions (~28m accuracy), visualized on a Leaflet map

## Hardware

| Component | Gateway | End Node |
|---|---|---|
| **Board** | LILYGO T-Beam SoftRF | LILYGO LoRa V1.4 |
| **MCU** | ESP32-S3 (240 MHz, 8 MB flash, 8 MB PSRAM) | ESP32 |
| **LoRa** | SX1278 (433 MHz, < -140 dBm sensitivity) | SX1278 |
| **GPS** | NEO-6M (50 channels, 2.5 m accuracy) | — |
| **PMU** | AXP2101 (battery management) | — |
| **Display** | — | 0.96" OLED (128x64) |

## Features

- LoRa packet reception with **Time of Arrival (TOA)** and **RSSI/SNR** extraction
- **NEO-6M GPS** positioning for gateway anchor coordinates
- **AXP2101 PMU** monitoring (battery voltage, percentage, charging status, temperature, VBUS)
- **MQTT** publishing to HiveMQ Cloud (TLS, port 8883)
- Full LoRa radio configuration at runtime via MQTT commands (frequency, SF, BW, CR, TX power, CRC, sync word, etc.)
- OTA firmware updates
- Home Assistant MQTT Discovery support for LoRa devices

## Quick Start

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- TTGO T-Beam board

### Build & Flash

```bash
pio run -e ttgo-t-beam -t upload
```

### Configuration

Edit `main/User_config.h` to set:

| Setting | Description |
|---|---|
| `WIFI_SSID` / `WIFI_PASSWORD` | WiFi credentials |
| `MQTT_SERVER` / `MQTT_PORT` | MQTT broker address (default: HiveMQ Cloud) |
| `MQTT_USER` / `MQTT_PASS` | MQTT authentication |
| `Gateway_Name` | Unique gateway identifier |

Module enables (in `User_config.h`):

```cpp
#define ZgatewayLORA   "LORA"   // LoRa gateway
#define ZsensorGPS     "GPS"    // GPS sensor
#define ZsensorPMU     "PMU"    // Power management
```

### LoRa Parameters

Configured via `main/config_LORA.h` or dynamically via MQTT topic `/commands/MQTTtoLORA/config`:

| Parameter | Default |
|---|---|
| Frequency | 433 MHz |
| TX Power | 20 dBm |
| Spreading Factor | 7 |
| Signal Bandwidth | 125 kHz |
| Coding Rate | 5 |
| Preamble Length | 8 |

## MQTT Topics

| Topic | Direction | Payload |
|---|---|---|
| `home/<gateway>/GPStoMQTT` | Publish | GPS position, satellites, altitude |
| `home/<gateway>/PMUtoMQTT` | Publish | Battery voltage, %, charging, temperature |
| `home/<gateway>/LORAtoMQTT` | Publish | LoRa packet data, RSSI, SNR, TOA |
| `home/<gateway>/commands/MQTTtoLORA` | Subscribe | Send data via LoRa |
| `home/<gateway>/commands/MQTTtoLORA/config` | Subscribe | Reconfigure LoRa radio |
| `home/<gateway>/SYStoMQTT` | Publish | Uptime, memory, WiFi status |

## Project Background

This firmware is part of a **bachelor's graduation project** — *Enhancing Pilgrim Safety: LoRa-Based Localization System for Precise Tracking in Makkah* — submitted to the Faculty of Engineering Technology, University of [Institution]. The system uses **TDOA multilateration** with a **Kalman filter** to achieve a mean localization error of ~28 meters.

## License

GPLv3 — inherited from [OpenMQTTGateway](https://github.com/1technophile/OpenMQTTGateway) which this firmware is forked from.
