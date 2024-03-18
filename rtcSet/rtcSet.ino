#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;

void setup() {
  Serial.begin(9600);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
}

void loop() {
  if (Serial.available() > 0) {
    String timeString = Serial.readStringUntil('\n'); // Read the time string
    setRTC(timeString);
  }
}

void setRTC(String timeString) {
  // Example timeString format: "2024-01-01T12:00:00"
  int Year = timeString.substring(0, 4).toInt();
  int Month = timeString.substring(5, 7).toInt();
  int Day = timeString.substring(8, 10).toInt();
  int Hour = timeString.substring(11, 13).toInt();
  int Minute = timeString.substring(14, 16).toInt();
  int Second = timeString.substring(17, 19).toInt();

  rtc.adjust(DateTime(Year, Month, Day, Hour, Minute, Second));
}
