#include <ModbusMaster.h>

// Initialize ModbusMaster object
ModbusMaster sensor;

// RS485 module control pins
#define MAX485_DE   3
#define MAX485_RE_NEG 2

// Sensor slave ID
#define SENSOR_SLAVE_ID 1

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
}

void loop() {
  int sensVal;
  
  // Read a single register that contains the CO2 concentration or the parameter of interest
  sensVal = sensor.readHoldingRegisters(256, 1);
  
  if (sensVal == sensor.ku8MBSuccess) {
    // Get the 16-bit signed integer from the response buffer
    int co2 = sensor.getResponseBuffer(0);
    Serial.print("CO2 = ");
    Serial.println(co2);
  } else {
    Serial.print("Failed to read from sensor, error code: ");
    Serial.println(sensVal);
  }
  
  // Delay between reads
  delay(1000);
}