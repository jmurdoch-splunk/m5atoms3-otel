#ifndef __DISTANCE_RCWL9620_H_
#define __DISTANCE_RCWL9620_H_

#include <Wire.h>
#include "../i2c/i2c-base.h"

#define I2C_ADDR_DISTANCE 0x57

void distanceInit(TwoWire *tw, uint8_t device);
float distanceGetCentimeters(TwoWire *tw, uint8_t device);

#endif