#include <SPI.h>
#include <SD.h>
#include "RTClib.h"

const int chipSelect = 10;
RTC_DS3231 rtc;
const int sampleWindow = 50;
unsigned int sample;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
}

void loop() {
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  
  // collect data for 50 ms
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(A0);
    if (sample < 1024) { 
      if (sample > signalMax) {
        signalMax = sample;  // save just the max levels
      } else if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }

  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 5.0) / 1024;  // convert to volts

  DateTime now = rtc.now();
  String ds = "";  
  ds.concat(now.unixtime());
  ds.concat(" ");
  ds.concat(now.year());
  ds.concat("-");
  ds.concat(now.month());
  ds.concat("-");
  ds.concat(now.day());
  ds.concat("@");
  ds.concat(now.hour());
  ds.concat(":");
  ds.concat(now.minute());
  ds.concat(":");
  ds.concat(now.second());
  ds.concat(" ");
  ds.concat(volts);

  Serial.println(ds);

  File dataFile = SD.open("data.log", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(ds);
    dataFile.close();
    // print to the serial port too:
  } else {
    Serial.println("error opening acoustic-data-logs.txt");
  }
  delay(1000);
}