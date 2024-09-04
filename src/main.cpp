#include <ArduinoBLE.h>
#include "ExtendedBLEDevice.h"
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFi.h>
#include <WiFiUdp.h>

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

const int timeZone = -3;     // UTC -3
//const int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)


WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
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
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
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

// Get MAC address without colons
String getMAC() {
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    return mac;
}

// Función para formatear la dirección MAC
String formatMacAddress(const String& macAddress) {
    String formattedMac = macAddress;
    formattedMac.replace(":", "");  // Elimina los dos puntos
    formattedMac.toUpperCase();      // Convierte a mayúsculas
    return formattedMac;
}

// Función para convertir datos de publicidad a una cadena hexadecimal
String convertToHexString(const uint8_t* data, int length) {
    String hexString;
    for (int i = 0; i < length; i++) {
        if (data[i] < 16) {
            hexString += '0';  // Agrega un 0 delante para valores menores a 16
        }
        hexString += String(data[i], HEX);  // Convierte a hexadecimal
    }
    hexString.toUpperCase();  // Convierte a mayúsculas
    return hexString;
}

void setup() {
    Serial.begin(115200);
    while (!Serial);  // Wait for serial port to connect

    // Setup WiFi using WiFiManager
    WiFiManager wm;
    String SSID = "ESP32-" + getMAC();
    bool connected = wm.autoConnect(SSID.c_str(), "password");

    if (!connected) {
        Serial.println("Failed to connect to WiFi");
        delay(100);
        ESP.restart(); // Restart ESP32 to begin again
    } else {
        Serial.println("Connected to WiFi");
    }

    Serial.print("IP number assigned by DHCP is ");
    Serial.println(WiFi.localIP());
    Serial.println("Starting UDP");
    Udp.begin(localPort);
    Serial.print("Local port: ");
    // Serial.println(Udp.localPort());
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
    setSyncInterval(300);


    // Start BLE
    if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
    }

    Serial.println("BLE Central scan");
    BLE.scan();
}

time_t prevDisplay = 0; // when the digital clock was displayed

void loop() {

    if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();
    }
    }

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
        String macAddress = peripheral.address();
        doc["mac"] = formatMacAddress(macAddress);  // Formatea la dirección MAC

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
            doc["advertisement_data"] = convertToHexString(advertisement, adLength);
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