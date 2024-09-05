#include "utilities.h"

WiFiUDP Udp;

// NTP Server
static const char ntpServerName[] = "us.pool.ntp.org";
const int timeZone = -3;  // Argentina Standard Time (UTC -3)
const int NTP_PACKET_SIZE = 48;
unsigned int localPort = 8888;

byte packetBuffer[NTP_PACKET_SIZE];

time_t getNtpTime() {
    IPAddress ntpServerIP;
    while (Udp.parsePacket() > 0);

    Serial.println("Transmit NTP Request");
    WiFi.hostByName(ntpServerName, ntpServerIP);
    Serial.print(ntpServerName);
    Serial.print(": ");
    Serial.println(ntpServerIP);
    sendNTPpacket(ntpServerIP);

    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
        int size = Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
            Serial.println("Receive NTP Response");
            Udp.read(packetBuffer, NTP_PACKET_SIZE);

            unsigned long secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
        }
    }
    Serial.println("No NTP Response :-(");
    return 0;
}

void sendNTPpacket(IPAddress &address) {
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011;
    packetBuffer[1] = 0;
    packetBuffer[2] = 6;
    packetBuffer[3] = 0xEC;
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    Udp.beginPacket(address, 123);
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}

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