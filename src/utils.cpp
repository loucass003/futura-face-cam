#include "FuturaFaceTracker.h"

uint16_t FuturaFaceTracker::getDeviceId()
{
#if defined(ARDUINO_ARCH_ESP32)
    return ESP.getEfuseMac();
#else
    return ESP.getChipId();
#endif
}

String FuturaFaceTracker::getDeviceName()
{
    return String("FuturaFaceTracker-") + String(this->getDeviceId(), HEX);
}