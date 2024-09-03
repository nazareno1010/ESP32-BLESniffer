// ExtendedBLEDevice.h
#pragma once

#include <ArduinoBLE.h>

class ExtendedBLEDevice : public BLEDevice {
public:
    ExtendedBLEDevice() : BLEDevice() {}
    ExtendedBLEDevice(const BLEDevice& device) : BLEDevice(device) {}

    int getAdvertisement(uint8_t value[], int length) {
        int adLength = advertisementDataLength();
        if (adLength > 0) {
            int copyLength = min(length, adLength);
            advertisementData(value, copyLength);
            return copyLength;
        }
        return 0;
    }
};