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
#define SENSOR_SLAVE_ID 1

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
  // Read a single register that contains the CO2 concentration or the parameter of interest
  int sensVal = sensor.readHoldingRegisters(256, 1);
  
  if (sensVal == sensor.ku8MBSuccess) {
    // Get the 16-bit signed integer from the response buffer
    int co2 = sensor.getResponseBuffer(0);
    Serial.print("CO2 = ");
    Serial.println(co2);

    // Save to SD card
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

      dataFile.print(", "); // delimiter between timestamp and data
      
      dataFile.println(co2);
      dataFile.close();
      Serial.println("Data written to SD card.");
    } else {
      Serial.println("Error opening datalog.txt");
    }
  } else {
    int co2 = -1000;
    Serial.print("CO2 = ");
    Serial.println(co2);

      // Save to SD card
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

      dataFile.print(", "); // delimiter between timestamp and data
      
      dataFile.println(co2);
      dataFile.close();
      Serial.println("Data written to SD card.");
    } else {
      Serial.println("Error opening datalog.txt");
    }
  }
  delay(1000); // delay 2 seconds
}