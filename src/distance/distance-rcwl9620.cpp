#include "distance-rcwl9620.h"

void distanceInit(TwoWire *tw, uint8_t device) {
    // Check ID
    // Serial.print("I2C WhoAmI: 0x");
    // Serial.println(readI2CSingleByte(tw, device, 0x75), HEX);
}

float distanceGetCentimeters(TwoWire *tw, uint8_t device) {
    uint32_t raw_range = 0;

    tw->beginTransmission(device);
    tw->write(0x01);
    tw->endTransmission();

    delay(150);

    tw->requestFrom(device, (uint8_t)3);
    raw_range = tw->read();
    raw_range <<= 8;
    raw_range += tw->read();
    raw_range <<= 8;
    raw_range += tw->read();
    return float(raw_range) / 1000;
}

