#ifndef NTP_H
#define NTP_H

#include <TimeLib.h>
#include <WiFi.h>
#include <WiFiUdp.h>

time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

extern WiFiUDP Udp;
extern unsigned int localPort;

#endif // NTP_H