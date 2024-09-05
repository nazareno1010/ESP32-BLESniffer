#ifndef UTILITIES_H
#define UTILITIES_H

#include <ArduinoBLE.h>
#include "ExtendedBLEDevice.h"
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Declaración de funciones
String getMAC();
String formatMacAddress(const String& macAddress);
String convertToHexString(const uint8_t* data, int length);
void printAdvertisementData(const uint8_t* data, int length);
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

extern WiFiUDP Udp;  // Declaración de la variable externa
extern unsigned int localPort;

#endif // UTILITIES_H
