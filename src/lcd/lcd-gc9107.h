#ifndef __LCD_GC9107_H_
#define __LCD_GC9107_H_

#include <Arduino.h>
#include "../qrcode/qrcode.h"

#define LCD_CSX     SS // SPI SS
#define LCD_SCL     SCK // SPI SCK
#define LCD_SDA     MOSI // SPI MOSI
#define LCD_DCX     GPIO_NUM_33 // LCD Data/Command
#define LCD_RESX    GPIO_NUM_34 // LCD Reset
#define LCD_BL      GPIO_NUM_16 // LCD Backlight (500MHz PWM)

void lcdCommand(int cmd);
void lcdCommandWrite(int cmd, const uint8_t *buf, size_t bytes, int datatype);
void lcdCommandWriteQRCode(int cmd, int qrcodeVer, char *text);
uint32_t lcdCommandRead(int cmd, size_t bytes);
void lcdResetHW(void);
void lcdInit(void);

#endif