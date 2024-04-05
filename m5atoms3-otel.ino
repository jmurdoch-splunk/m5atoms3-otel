// OOTB Arduino / Platform libraries
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Core custom libraries
#include "src/customOtel/otelProtobuf/otelProtobuf.h"
#include "src/customOtel/sendProtobuf/sendProtobuf.h"
#include "src/imu/imu-mpu6886.h"
#include "src/lcd/lcd-gc9107.h"

// Optional custom libraries
// #include "src/distance/distance-rcwl9620.h"

// Libraries in build dir
#include "splunk_logo.h"
#include "connectionDetails.h"

// Untested - can send IR codes
#define IR_TX       GPIO_NUM_4

// Screen button
#define BUTTON      GPIO_NUM_41

// Internal I2C, hooked up to the MPU-6886 IMU
#define I2C_INT_SCL SCL
#define I2C_INT_SDA SDA
TwoWire I2C_INT = TwoWire(0);

/*
 * Three choices for external port:
 * - Raw GPIO (1 & 2)
 * - Serial (Rx & Tx)
 * - I2C (SCL & SDA)
 */
// e.g. Key Unit, Scales
#define PORT_A_WHITE GPIO_NUM_1 // Typically an Arduino input
#define PORT_A_YELLOW GPIO_NUM_2 // Typically a Arduino output

// e.g. GPS
#define UART_EXT_TXD GPIO_NUM_1
#define UART_EXT_RXD GPIO_NUM_2

// e.g. Ultrasonic Distance, Ambient Light
#define I2C_EXT_SCL GPIO_NUM_1
#define I2C_EXT_SDA GPIO_NUM_2

// External I2C
TwoWire I2C_EXT = TwoWire(1);

/*
 * Setup various connectivity pieces
 */
// WiFi
const char *ssid = WIFISSID;
const char *pass = WIFIPSK;
// NTP
const char *ntphost = NTPHOST;
// OpenTelemetry Endpoint
char *host = OTELHOST;
int port = OTELPORT;
char *uri = OTELURI;
char *xsfkey = XSFKEY;

const int pinButton = BUTTON;

void gatherMetrics(void) {
    uint8_t payloadData[MAX_PROTOBUF_BYTES] = { 0 };

    // Create the data store with data structure (default)
    Resourceptr ptr = NULL;
    ptr = addOteldata();

    addResAttr(ptr, "service.name", "atoms3-poc");

    addMetric(ptr, "temperature", "cpu temperature", "1");
    addDatapoint(ptr, AS_DOUBLE, imuTemp(&I2C_INT, I2C_ADDR_IMU));

    addMetric(ptr, "accelerometer", "g forces applied to device", "1");
    addDatapoint(ptr, AS_DOUBLE, imuAccel(&I2C_INT, I2C_ADDR_IMU, AXIS_X));
    addDpAttr(ptr, "axis", "x");
    addDatapoint(ptr, AS_DOUBLE, imuAccel(&I2C_INT, I2C_ADDR_IMU, AXIS_Y));
    addDpAttr(ptr, "axis", "y");
    addDatapoint(ptr, AS_DOUBLE, imuAccel(&I2C_INT, I2C_ADDR_IMU, AXIS_Z));
    addDpAttr(ptr, "axis", "z");

    addMetric(ptr, "gyroscope", "angular velocity applied to device", "1");
    addDatapoint(ptr, AS_DOUBLE, imuGyro(&I2C_INT, I2C_ADDR_IMU, AXIS_X));
    addDpAttr(ptr, "axis", "x");
    addDatapoint(ptr, AS_DOUBLE, imuGyro(&I2C_INT, I2C_ADDR_IMU, AXIS_Y));
    addDpAttr(ptr, "axis", "y");
    addDatapoint(ptr, AS_DOUBLE, imuGyro(&I2C_INT, I2C_ADDR_IMU, AXIS_Z));
    addDpAttr(ptr, "axis", "z");

    printOteldata(ptr);
    
    size_t payloadSize = buildProtobuf(ptr, payloadData, MAX_PROTOBUF_BYTES);
    // Send the data if there's something there
    if(payloadSize > 0) {
        sendProtobuf(host, port, uri, xsfkey, payloadData, payloadSize);
    } 

    // Free the data store
    freeOteldata(ptr);
}


void setup() {
    // Wait for boot messages to complete
    delay(16);
 
    // USB CDC On Boot: Enabled for Serial to work
    Serial.begin(115200);

    // Connect WiFi
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);
    }

    // NTP Sync
    struct tm tm;
    time_t t;
    struct timeval tv;
    configTime(0, 0, ntphost);

    // Get time from NTP
    getLocalTime(&tm);

    // Set time on HW clock
    t = mktime(&tm);
    tv = { .tv_sec = t };
    settimeofday(&tv, NULL);

    /*
     * Button 
     */
    pinMode(BUTTON, INPUT_PULLUP);

    /*
     * LCD Screen 
     */
    lcdInit();

    // Print Splunk logo
    // lcdCommandWrite(0x2C, splunklogo, sizeof(splunklogo), 1);

    // Print QRCode 
    lcdCommandWriteQRCode(0x2C, 3, "google.com");

    /*
     * I2C for IMU (Accelerometer, Gyroscope, Temperature)
     */
    I2C_INT.begin(I2C_INT_SDA, I2C_INT_SCL, 100000);
    imuInit(&I2C_INT, I2C_ADDR_IMU);

    /*
     * External
     */
    // pinMode(PORT_A_YELLOW, OUTPUT);
    // pinMode(PORT_A_WHITE, INPUT);

    // I2C_EXT.begin(I2C_EXT_SDA, I2C_EXT_SCL, 100000);
    // distanceInit(&I2C_EXT, I2C_ADDR_DISTANCE);
}

void loop() {
    delay(1000);

    gatherMetrics();
    
    Serial.print("Internal button state: ");
    Serial.println(digitalRead(BUTTON));

    // Serial.print("Distance: ");
    // Serial.println(distanceGetCentimeters(&I2C_EXT, I2C_ADDR_DISTANCE));
}