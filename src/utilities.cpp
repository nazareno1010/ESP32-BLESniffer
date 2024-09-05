#include "utilities.h"

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