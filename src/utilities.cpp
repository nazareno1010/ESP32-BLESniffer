#include "utilities.h"
#include "mqtt.h"

WiFiClient espClient;                // Definición de la variable
PubSubClient mqtt_client(espClient);

String getMAC() {
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    return mac;
}

String formatMacAddress(const String& macAddress) {
    String formattedMac = macAddress;
    formattedMac.replace(":", "");
    formattedMac.toUpperCase();
    return formattedMac;
}

String convertToHexString(const uint8_t* data, int length) {
    String hexString;
    for (int i = 0; i < length; i++) {
        if (data[i] < 16) {
            hexString += '0';
        }
        hexString += String(data[i], HEX);
    }
    hexString.toUpperCase();
    return hexString;
}

void printAdvertisementData(const uint8_t* data, int length) {
    Serial.print("Raw data: ");
    for (int i = 0; i < length; i++) {
        Serial.print(data[i] < 16 ? "0" : "");
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

void connectToMQTT() {
    while (!mqtt_client.connected()) {
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            mqtt_client.publish(mqtt_topic, "Hi Pablo, I'm ESP32 ^.^"); // Publish message upon successful connection
        } else {
            Serial.print("Failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void mqttCallback(char *mqtt_topic, byte *payload, unsigned int length) {
    Serial.print("Message received on mqtt_topic: ");
    Serial.println(mqtt_topic);
    Serial.print("Message: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println("\n-----------------------");
}