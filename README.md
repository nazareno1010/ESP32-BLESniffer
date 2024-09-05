# ESP32 BLE and MQTT Integration

## Overview

This project demonstrates how to use an ESP32 microcontroller to scan for Bluetooth Low Energy (BLE) devices and publish their information to an MQTT broker. The information is formatted as a JSON object and includes details such as timestamp, MAC address, RSSI, and advertisement data of the discovered BLE devices.

## Features

- **BLE Scanning**: The ESP32 scans for nearby BLE devices.
- **JSON Formatting**: Information about BLE devices is formatted into JSON.
- **MQTT Publishing**: The JSON data is published to an MQTT broker.
- **NTP Time Synchronization**: The ESP32 synchronizes its time using an NTP server.
- **WiFi Management**: The ESP32 connects to a WiFi network using WiFiManager.

## Libraries Used

- `ArduinoBLE` for BLE functionality.
- `ArduinoJson` for JSON handling.
- `TimeLib` for time management.
- `WiFiManager` for handling WiFi connections.
- `PubSubClient` for MQTT communication.

## Setup

1. **WiFi Connection**:
   - The ESP32 connects to a WiFi network using WiFiManager.
   - SSID is automatically generated using the MAC address of the ESP32.

2. **MQTT Configuration**:
   - The MQTT client is configured to connect to a specified MQTT broker.
   - Ensure to set `mqtt_broker`, `mqtt_port`, and `mqtt_topic` in your `mqtt.h` file.

3. **Time Synchronization**:
   - The ESP32 synchronizes its time with an NTP server.
   - The time is used to timestamp the BLE data.

4. **BLE Scanning**:
   - The ESP32 starts scanning for BLE devices.
   - Information from detected devices is processed and published via MQTT.

## Code Description

- **`setup()` Function**:
  - Initializes serial communication.
  - Connects to WiFi.
  - Sets up MQTT client and connects to the MQTT broker.
  - Starts BLE scanning.
  - Configures NTP time synchronization.

- **`loop()` Function**:
  - Checks for available BLE devices.
  - If a BLE device is found, its details are collected and formatted into a JSON object.
  - The JSON object is serialized to a string and published to the MQTT broker.
  - Keeps the MQTT connection alive and processes incoming MQTT messages.

## Notes

- Ensure that your MQTT broker is accessible and that you have the correct credentials and topic configured.
- Adjust the size of the `JsonDocument` if you need to accommodate larger JSON objects.

