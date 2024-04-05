
#include "lcd-gc9107.h"

void lcdCommand(int cmd) {
    pinMode(LCD_SDA, OUTPUT);
    digitalWrite(LCD_DCX, LOW);
    for (int i = 0; i < 8; i++) {
        digitalWrite(LCD_SDA, cmd & 0x80);
        delay(1);
        digitalWrite(LCD_SCL, HIGH);
        delay(1);
        digitalWrite(LCD_SCL, LOW);
        cmd <<= 1;
    }
}

void lcdCommandWrite(int cmd, const uint8_t *buf, size_t bytes, int datatype) {
    // Open SPI
    digitalWrite(LCD_CSX, LOW);

    lcdCommand(cmd);

    // Data Write
    pinMode(LCD_SDA, OUTPUT); 
    digitalWrite(LCD_DCX, HIGH);
    for (int x = 0; x < bytes; x++) {
        // MSBFIRST
        for (int y = 7; y >= 0; y--) {
            if (datatype == 0)
                digitalWrite(LCD_SDA, (buf[x] >> y) & 0x1);
            else if (datatype == 1)
                digitalWrite(LCD_SDA, (pgm_read_byte_near(buf + x) >> y) & 0x1);
            else if (datatype == 2)
                digitalWrite(LCD_SDA, (buf[0] >> y) & 0x1);

            digitalWrite(LCD_SCL, HIGH);
            digitalWrite(LCD_SCL, LOW);
        }
    }

    // Close SPI
    digitalWrite(LCD_CSX, HIGH);
}

void lcdCommandWriteQRCode(int cmd, int qrcodeVer, char *text) {
    /*
     *  QRCode versions work out okay:
     *  - 3: 29x29
     *  - 6: 41x41
     *  - 11: 61x61
     */
    int dispRes = 128;

    int codeRes = 4 * qrcodeVer + 17;
    int scale = dispRes / codeRes;

    // Quiet Zones Calculation
    int qTopLeft = (dispRes - (codeRes * scale)) / 2;
    int qBottomRight = (dispRes - (codeRes * scale)) - qTopLeft;

    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(qrcodeVer)];
    qrcode_initText(&qrcode, qrcodeData, qrcodeVer, 0, text);
    
    // Open SPI
    digitalWrite(LCD_CSX, LOW);

    lcdCommand(cmd);

    // Data Write
    pinMode(LCD_SDA, OUTPUT); 
    digitalWrite(LCD_DCX, HIGH);
    // Display
    uint8_t pixel = 0x00;
    for (int addr = 0; addr < 16384; addr++) {
        // Quiet Zone - Left, Right, Top, Bottom
        if ((addr % dispRes) < qTopLeft || (addr % dispRes) >= (dispRes - qBottomRight) || addr < (dispRes * qTopLeft) || addr >= (dispRes * dispRes) - (dispRes * qBottomRight)) {
            pixel = 0xFF;
        } else {
            if (qrcode_getModule(&qrcode, ((addr % dispRes) - qTopLeft) / scale, ((addr / dispRes) - qTopLeft) / scale)) {
                pixel = 0x00;
            } else {
                pixel = 0xFF;
            }
        }
        // RGB - Greyscale, so no subprocessing
        for (int rgb = 0; rgb < 3; rgb++) {
            // 8 bits
            for (int bit = 7; bit >= 0; bit--) {
                digitalWrite(LCD_SDA, (pixel >> bit) & 0x1);
                digitalWrite(LCD_SCL, HIGH);
                digitalWrite(LCD_SCL, LOW);
            }
        }
    }

    // Close SPI
    digitalWrite(LCD_CSX, HIGH);
}

uint32_t lcdCommandRead(int cmd, size_t bytes) {
    // GC9107 Datasheet - Section 4.17
    uint32_t data = 0x0;

    // Open SPI
    digitalWrite(LCD_CSX, LOW);

    // Command Write
    lcdCommand(cmd);

    if (bytes > 0) {
        // Data Mode
        pinMode(LCD_SDA, INPUT_PULLUP); 
        digitalWrite(LCD_DCX, HIGH);

        // If > 8 bits, we need a dummy clock cycle
        if (bytes > 1) {
            digitalWrite(LCD_SCL, HIGH);
            digitalWrite(LCD_SCL, LOW);
        }
  
        // Read all bits, MSBFIRST
        for (int i = 0; i < (bytes * 8); i++) {
            data <<= 1;
            data |= digitalRead(LCD_SDA);
            digitalWrite(LCD_SCL, HIGH);
            digitalWrite(LCD_SCL, LOW);
        }
    }

    // Close SPI
    digitalWrite(LCD_CSX, HIGH);

    return data;
}

void lcdResetHW(void)
{
    digitalWrite(LCD_RESX, HIGH);
    digitalWrite(LCD_RESX, LOW);
    delay(120);
    digitalWrite(LCD_RESX, HIGH);
    delay(10);
}

void lcdInit(void) {
    // LCD SPI Pins
    digitalWrite(LCD_CSX, HIGH);
    pinMode(LCD_CSX, OUTPUT);
    pinMode(LCD_SCL, OUTPUT);
    pinMode(LCD_SDA, OUTPUT);

    // LCD Other Pins
    pinMode(LCD_DCX, OUTPUT);
    pinMode(LCD_RESX, OUTPUT);
    pinMode(LCD_BL, OUTPUT);

    // Switch on backlight
    analogWrite(LCD_BL, 180);


    lcdResetHW();             // Reset LCD at hardware level
    
    /*
     * Simple Configuration
     */
    lcdCommandRead(0x01, 0);    // C: Software Reset
    delay(150);
    if( lcdCommandRead(0x04, 3) == 0x9107 ) // Check Screen ID
        Serial.println(F("Screen OK"));
    delay(10);
    lcdCommandRead(0x21, 0);    // C: Invert
    delay(150);
    lcdCommandRead(0x11, 0);    // C: Sleep Out
    delay(500);       
    lcdCommandRead(0x29, 0);    // C: Display ON
    delay(500);

    /*
     * Advanced Configuration
     */
    // Offset screen by 2 on X
    const uint8_t colset[] = {0x00, 0x02, 0x00, 0x81};
    lcdCommandWrite(0x2A, colset, sizeof(colset), 0); 
    delay(10);

    // Offset screen by 1 on y
    const uint8_t rowset[] = {0x00, 0x01, 0x00, 0x80};
    lcdCommandWrite(0x2B, rowset, sizeof(rowset), 0); 
    delay(10);

    // rotate 180, convert RGB to BGR
    const uint8_t memctl[] = { 0xC8 }; 
    lcdCommandWrite(0x36, memctl, sizeof(memctl), 0); 
    delay(10);
    
    // Fill screen with Black
    const uint8_t black[] = { 0x00 };
    lcdCommandWrite(0x2C, black, 49152, 2);
}