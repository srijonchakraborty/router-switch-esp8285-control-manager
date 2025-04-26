# Router & Switch Control Manager for ESP8285

![ESP8285](https://img.shields.io/badge/ESP-8285-orange)
![WiFi](https://img.shields.io/badge/WiFi-Dual%20Mode-blue)

A robust IoT solution for remotely managing network devices using ESP8285 with web interface and automatic monitoring.

https://github.com/user-attachments/assets/f8633578-1203-4118-abf9-22c5ce467007

## Table of Contents
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Installation](#installation)
- [Usage](#usage)
- [API Documentation](#api-documentation)


## Features

### Dual Network Operation
- **WiFi Client Mode**: Connects to existing network
- **Access Point Mode**: Creates backup hotspot (AP) when primary fails

### Device Control
- Web-based control for router power (GPIO4)
- Web-based control for switch power (GPIO5)
- Get Fed From Transistor in ADC Pin
- Visual status indicators for all connected devices

### Smart Monitoring
- Automatic internet connectivity verification
- Multi-endpoint testing (Google, Microsoft and other)
- 5-minute check interval (configurable)

## Hardware Requirements

| Component | Specification |
|-----------|---------------|
| MCU | ESP8285 module |
| Relays | 2x 5V/10A relay modules |
| Power | 3.3V for ESP, appropriate for relays |
| Interface | Micro-USB for programming |

## Installation

### Prerequisites
- Arduino IDE with ESP8266 core installed
- Or PlatformIO environment
- Install CH34x_Install_Windows_v3_4 Driver (Google it and you will find it)
   Without the Driver Esp8285 Will not work with arduino ide because PC will detact this device as a unknown device.

## Usage
### Web Interface

Access the control panel via:
    - AP Mode: http://192.168.4.1

## Control Options
   - **Router Control**: Turn router on/off
   - **Switch Control**: Turn secondary switch on/off
   - **Manual Refresh**: Force status update
   - **Connection Info**: View network details

## API Documentation
### Control Endpoints

| Endpoint | Method | Description |
|-----------|-------|-------------|
| /router-on | GET | Powers on router |
| /router-off | GET | Powers on router |
| /switch-on | GET | Powers on router |
| /switch-off | GET | Powers on router |

![Project Image](https://github.com/user-attachments/assets/34253ae6-1fde-4935-a60a-849df2acc8d3)

![Transistor Circuit Diagram](https://github.com/user-attachments/assets/0877430d-f060-4f1b-ae82-84a8e514e2ec)

![Transistor Circuit](https://github.com/user-attachments/assets/b0d4986c-8d01-43c5-b946-860154760615)
| /status | GET | Check Power On or Off |
| /check-internet | GET | check internet is working or not |


