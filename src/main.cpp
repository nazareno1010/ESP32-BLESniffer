#include <ArduinoBLE.h>
#include "ExtendedBLEDevice.h"

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