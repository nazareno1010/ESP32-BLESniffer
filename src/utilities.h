#ifndef UTILITIES_H
#define UTILITIES_H

#include <ArduinoBLE.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

String getMAC();
String formatMacAddress(const String& macAddress);
String convertToHexString(const uint8_t* data, int length);
void printAdvertisementData(const uint8_t* data, int length);
void connectToMQTT();
void mqttCallback(char *mqtt_topic, byte *payload, unsigned int length);

time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

extern WiFiUDP Udp;
extern WiFiClient espClient;
extern PubSubClient mqtt_client;
extern unsigned int localPort;

#endif // UTILITIES_H