#include <WiFiManager.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoBLE.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

#include "ExtendedBLEDevice.h"
#include "utilities.h"
#include "mqtt.h"

// Needed to add blacklist, rssi parameter, mqtt credentials, time limit

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Connect to WiFi using WiFiManager
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

    Serial.println("Starting UDP");
    Udp.begin(localPort);
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
    setSyncInterval(300);
    
    // Connect to MQTT using PubSubClient
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    mqtt_client.setKeepAlive(60);
    mqtt_client.setCallback(mqttCallback);
    connectToMQTT();

    // Connect to BLE using ArduinoBLE
    if (!BLE.begin()) {
        while (1);
    }
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

        // if (peripheral.hasLocalName()) {
        //     doc["localname"] = peripheral.address();
        // }

        doc["rssi"] = peripheral.rssi();

        if (peripheral.hasAdvertisedServiceUuid()) {
            JsonArray uuids = doc["service_uuids"].to<JsonArray>();
            for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
                uuids.add(peripheral.advertisedServiceUuid(i));
            }
        }

        uint8_t advertisement[62] = {0};
        int adLength = peripheral.getAdvertisement(advertisement, sizeof(advertisement));
        // doc["advertisement_data_length"] = adLength;

        if (adLength > 0) {
            doc["rawData"] = convertToHexString(advertisement, adLength);
        } // else {
        //     doc["advertisement_data"] = "No advertisement data available";
        // }

        String jsonString;
        serializeJson(doc, jsonString);


        // Connect to MQTT Broker
        if (!mqtt_client.connected()) {
            connectToMQTT();
        }
        mqtt_client.loop();

        mqtt_client.publish(mqtt_topic, jsonString.c_str());

        // Continue BLE Scan
        BLE.poll();
    }
}