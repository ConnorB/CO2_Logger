#include <ModbusMaster.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>   // Required for I2C communication with the RTC
#include <RTClib.h> // Include the RTC library

// Initialize ModbusMaster object
ModbusMaster sensor;

// RS485 module control pins
#define MAX485_DE   3
#define MAX485_RE_NEG 2

// Sensor slave ID
#define SENSOR_SLAVE_ID 240

// SD Card pin
const int chipSelect = 10;

// Create an RTC object
RTC_DS1307 rtc;

// Define File
File dataFile;

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission() {
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup() {
  // Start serial communication
  Serial.begin(19200);

  // Initialize control pins
  pinMode(MAX485_DE, OUTPUT);
  pinMode(MAX485_RE_NEG, OUTPUT);

  // Initialize Modbus communication
  sensor.begin(SENSOR_SLAVE_ID, Serial);
  sensor.preTransmission(preTransmission);
  sensor.postTransmission(postTransmission);

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed for SD card!");
    return;
  }
  Serial.println("SD card initialization done.");

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
}

void loop() {
  // Reading Temperature from Modbus address 4
  uint8_t tempResult = sensor.readHoldingRegisters(4, 2);
  float temperature = 0.0;
  if (tempResult == sensor.ku8MBSuccess) {
    uint16_t tempLow = sensor.getResponseBuffer(0);
    uint16_t tempHigh = sensor.getResponseBuffer(1);
    uint32_t tempRaw = ((uint32_t)tempHigh << 16) | tempLow;
    temperature = *((float*)&tempRaw);
    Serial.print("Temperature (C) = ");
    Serial.println(temperature);
  } else {
    Serial.println("Error reading temperature");
  }
delay(50);
  // Reading CO2 from Modbus address 0
  uint8_t co2Result = sensor.readHoldingRegisters(0, 2);
  float co2 = 0.0;
  if (co2Result == sensor.ku8MBSuccess) {
    uint16_t co2Low = sensor.getResponseBuffer(0);
    uint16_t co2High = sensor.getResponseBuffer(1);
    uint32_t co2Raw = ((uint32_t)co2High << 16) | co2Low;
    co2 = *((float*)&co2Raw);
    Serial.print("CO2 (ppm) = ");
    Serial.println(co2);
  } else {
    Serial.println("Error reading CO2");
  }

  // Save to SD card, logging both temperature and CO2
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    // write timestamp
    DateTime now = rtc.now();
    dataFile.print(now.year(), DEC);
    dataFile.print('-');
    dataFile.print(now.month(), DEC);
    dataFile.print('-');
    dataFile.print(now.day(), DEC);
    dataFile.print(' ');
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);

    dataFile.print(", "); // Label the temperature data
    dataFile.print(temperature);
    dataFile.print(", "); // Label the CO2 data
    dataFile.println(co2);
    
    dataFile.close();
    Serial.println("Data written to SD card.");
  } else {
    Serial.println("Error opening datalog.txt");
  }
  
  delay(5000); // delay 2 seconds
}
