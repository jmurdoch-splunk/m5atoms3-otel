#ifndef __IMU_MPU6886_H_
#define __IMU_MPU6886_H_

#include <Arduino.h> // For Serial debugging
#include <Wire.h>
#include "../i2c/i2c-base.h"

#define I2C_ADDR_IMU 0x68

enum { AXIS_X = 0, AXIS_Y = 2, AXIS_Z = 4 };

double* imuTemp(TwoWire *tw, uint8_t device);
double* imuAccel(TwoWire *tw, uint8_t device, int axis);
double* imuGyro(TwoWire *tw, uint8_t device, int axis);
void imuInit(TwoWire *tw, uint8_t device);

#endif