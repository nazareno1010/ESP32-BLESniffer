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

        // Add UUID
        if (peripheral.hasAdvertisedServiceUuid()) {

          doc["uuid"] = peripheral.rssi(); // Needed for update

          for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
            Serial.print(peripheral.advertisedServiceUuid(i));
            Serial.print(" ");
          }
          Serial.println();
        }

        // Add raw data
        String rawData = "";
        for (int i = 0; i < peripheral.advertisementDataLength(); i++) {
            char hexByte[3];
            snprintf(hexByte, sizeof(hexByte), "%02X", peripheral.advertisementData()[i]);
            rawData += String(hexByte) + " ";
        }
        doc["rawData"] = rawData;

        // Serialize JSON to Serial
        serializeJson(doc, Serial);
        Serial.println();
    }

    // Maintain BLE scanning
    BLE.poll();
}

void loop() {
  BLEDevice device = BLE.available();
  
  if (device) {
    ExtendedBLEDevice peripheral(device);
    
    Serial.println("\nDiscovered a peripheral");
    Serial.println("-----------------------");
    
    Serial.print("Address: ");
    Serial.println(peripheral.address());
    
    
    if (peripheral.hasLocalName()) {
      Serial.print("Local Name: ");
      Serial.println(peripheral.localName());
    }
    
    if (peripheral.hasAdvertisedServiceUuid()) {
      Serial.print("Service UUIDs: ");
      for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
        Serial.print(peripheral.advertisedServiceUuid(i));
        Serial.print(" ");
      }
      Serial.println();
    }
    
    Serial.print("RSSI: ");
    Serial.println(peripheral.rssi());
    
    uint8_t advertisement[62] = {0};  // Increased buffer size
    int adLength = peripheral.getAdvertisement(advertisement, sizeof(advertisement));
    
    Serial.print("Advertisement data length: ");
    Serial.println(adLength);
    
    if (adLength > 0) {
      printAdvertisementData(advertisement, adLength);
    } else {
      Serial.println("No advertisement data available");
    }
    
    Serial.println();
    delay(1000);  // Add a small delay to avoid flooding the serial output
  }
}