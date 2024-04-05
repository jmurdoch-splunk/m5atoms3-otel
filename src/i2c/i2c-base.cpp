#include "i2c-base.h"

uint8_t readI2CSingleByte(TwoWire *tw, uint8_t device, uint8_t addr) {
  uint8_t value = 0;
  tw->beginTransmission(device);
  tw->write(addr);
  tw->endTransmission(false);

  tw->requestFrom(device, (uint8_t)1, (uint8_t)1);
  if(tw->available())
    value = tw->read();
  tw->endTransmission(device); 
  return value;  
}

void readI2CConsecutiveBytes(TwoWire *tw, uint8_t device, uint8_t addr, uint8_t *values, uint8_t size) {
  tw->beginTransmission(device);
  tw->write(addr);
  tw->endTransmission(false);

  tw->requestFrom(device, size, (uint8_t)1);
  for (int x = 0; x < size; x++) {
    values[x] = tw->read();
  }
  tw->endTransmission(device); 
}

void writeI2CSingleByte(TwoWire *tw, uint8_t device, uint8_t addr, uint8_t value) {
  tw->beginTransmission(device);
  tw->write(addr);
  tw->write(value);
  tw->endTransmission(device);
}
