#ifndef __I2C_BASE_H_
#define __I2C_BASE_H_

#include <Wire.h>

uint8_t readI2CSingleByte(TwoWire *tw, uint8_t device, uint8_t addr);
void readI2CConsecutiveBytes(TwoWire *tw, uint8_t device, uint8_t addr, uint8_t *values, uint8_t size);
void writeI2CSingleByte(TwoWire *tw, uint8_t device, uint8_t addr, uint8_t value);

#endif