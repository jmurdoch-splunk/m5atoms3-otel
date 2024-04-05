#include "imu-mpu6886.h"

double* imuTemp(TwoWire *tw, uint8_t device) {
    double *temp = (double *)malloc(sizeof(double)); 
    *temp = ((readI2CSingleByte(tw, device, 0x41) << 8) | readI2CSingleByte(tw, device, 0x42)) / 326.8 + 25.0;
    return temp;
}

double* imuAccel(TwoWire *tw, uint8_t device, int axis) {
    double *accel = (double *)malloc(sizeof(double)); 
    *accel = (int16_t)((readI2CSingleByte(tw, device, 0x3b + axis) << 8) | readI2CSingleByte(tw, device, 0x3c + axis)) * 8.0 / 32768.0;
    return accel;
}

double* imuGyro(TwoWire *tw, uint8_t device, int axis) {
    double *gyro = (double *)malloc(sizeof(double)); 
    *gyro = (int16_t)((readI2CSingleByte(tw, device, 0x43 + axis) << 8) | readI2CSingleByte(tw, device, 0x44 + axis)) * 2000.0 / 32768.0;
    return gyro;
}

void imuInit(TwoWire *tw, uint8_t device) {
    // Check ID
    Serial.print("I2C WhoAmI: 0x");
    Serial.println(readI2CSingleByte(tw, device, 0x75), HEX);

    // Power Management 1
    writeI2CSingleByte(tw, device, 107, 0x00);
    delay(10);
    writeI2CSingleByte(tw, device, 107, 0x80); // Device Reset
    delay(10);
    writeI2CSingleByte(tw, device, 107, 0x01); // Full Gyro perf clock source
    delay(10);

    // Acceleration Configuration
    writeI2CSingleByte(tw, device, 28, 0x10); // +/- 8G
    delay(1);

    // Gyroscope Configuration
    writeI2CSingleByte(tw, device, 27, 0x18); // +/- 2000dps
    delay(1);

    // Configuration
    writeI2CSingleByte(tw, device, 26, 0x01); // FIFO replace, no FSYNC, basic low-pass filter
    delay(1);

    // Sample Rate Divider
    writeI2CSingleByte(tw, device, 25, 0x05); // Divide low-pass filter by 6
    delay(1);

    // Interrupt enable
    writeI2CSingleByte(tw, device, 56, 0x00); // No interrupts
    delay(1);

    // Accelerometer Configuration 2
    writeI2CSingleByte(tw, device, 29, 0x00); // 4 samples, nominal rate
    delay(1);

    // User Control
    writeI2CSingleByte(tw, device, 106, 0x00); // Disable FIFO access
    delay(1);

    // FIFO Enable
    writeI2CSingleByte(tw, device, 35, 0x00); // Disable FIFO
    delay(1);

    // Interrupt/DataReady PIN & Bypass Enable
    writeI2CSingleByte(tw, device, 55, 0x22); // Clear interrupt on read
    delay(1);

    // Interrupt Enable
    writeI2CSingleByte(tw, device, 56, 0x01); // Enable
    delay(100);
}