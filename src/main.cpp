#include <ArduinoBLE.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include "utilities.h"
#include "ExtendedBLEDevice.h"

void setup() {
    Serial.begin(115200);
    while (!Serial);

    WiFiManager wm;
    String SSID = "ESP32-" + getMAC();
    bool connected = wm.autoConnect(SSID.c_str(), "password");

    if (!connected) {
        Serial.println("Failed to connect to WiFi");
        delay(100);
        ESP.restart();
    } else {
        Serial.println("Connected to WiFi");
    }

    Serial.print("IP number assigned by DHCP is ");
    Serial.println(WiFi.localIP());
    Serial.println("Starting UDP");
    Udp.begin(localPort);
    Serial.print("Local port: ");
    Serial.println(localPort);
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
    setSyncInterval(300);

    if (!BLE.begin()) {
        Serial.println("starting BLE failed!");
        while (1);
    }

    Serial.println("BLE Central scan");
    BLE.scan();
}

void loop() {
    BLEDevice device = BLE.available();

    if (device) {
        ExtendedBLEDevice peripheral(device);

        JsonDocument doc;
        char timestamp[25];
        snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
                 year(), month(), day(), hour(), minute(), second());
        doc["timestamp"] = timestamp;
        doc["mac"] = formatMacAddress(peripheral.address());

        if (peripheral.hasLocalName()) {
            doc["localname"] = peripheral.address();
        }

        doc["rssi"] = peripheral.rssi();

        if (peripheral.hasAdvertisedServiceUuid()) {
            JsonArray uuids = doc["service_uuids"].to<JsonArray>();
            for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
                uuids.add(peripheral.advertisedServiceUuid(i));
            }
        }

        uint8_t advertisement[62] = {0};
        int adLength = peripheral.getAdvertisement(advertisement, sizeof(advertisement));
        doc["advertisement_data_length"] = adLength;

        if (adLength > 0) {
            doc["advertisement_data"] = convertToHexString(advertisement, adLength);
        } else {
            doc["advertisement_data"] = "No advertisement data available";
        }

        serializeJson(doc, Serial);
        Serial.println();
    }

    BLE.poll();
}