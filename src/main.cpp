#include <ArduinoBLE.h>
#include "ExtendedBLEDevice.h"
#include <ArduinoJson.h>
#include <TimeLib.h>

void printAdvertisementData(const uint8_t* data, int length) {
  Serial.print("Raw data: ");
  for (int i = 0; i < length; i++) {
    Serial.print(data[i] < 16 ? "0" : "");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

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

        // Create a JSON object
        StaticJsonDocument<256> doc;

        // Add timestamp
        char timestamp[25];
        snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
                 year(), month(), day(), hour(), minute(), second());
        doc["timestamp"] = timestamp;

        // Add MAC address
        doc["mac"] = peripheral.address();

        // Add Local Name
        if (peripheral.hasLocalName()) {
          doc["localname"] = peripheral.address();
        }

        // Add RSSI
        doc["rssi"] = peripheral.rssi();

        // Add Service UUIDs
        if (peripheral.hasAdvertisedServiceUuid()) {
            JsonArray uuids = doc.createNestedArray("service_uuids");
            for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
                uuids.add(peripheral.advertisedServiceUuid(i));
            }
        }

        // Add Advertisement Data
        uint8_t advertisement[62] = {0};  // Increased buffer size
        int adLength = peripheral.getAdvertisement(advertisement, sizeof(advertisement));
        doc["advertisement_data_length"] = adLength;

        if (adLength > 0) {
            JsonArray adData = doc.createNestedArray("advertisement_data");
            for (int i = 0; i < adLength; i++) {
                adData.add(advertisement[i]);
            }
        } else {
            doc["advertisement_data"] = "No advertisement data available";
        }

        // Serialize JSON to Serial
        serializeJson(doc, Serial);
        Serial.println();
    }

    // Maintain BLE scanning
    BLE.poll();
}