  
const int buttonPin = 7;
int buttonState = 0; 

#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads(0x48);

float pressure = 0.0;
int Pino_CS = 10;

RTC_DS1307 rtc;
File file;

void setup() {
   Serial.begin(9600);
   ads.begin();
   ads.setGain(GAIN_EIGHT);
   pinMode(6, OUTPUT); 
   pinMode(5, OUTPUT);
   pinMode(buttonPin, INPUT);      
   Serial.println("Data logger com modulo PN532");
   Serial.println();
   Serial.println("Starting SD...");
  if (!SD.begin(Pino_CS))
  {
    Serial.println("error to start SD!");
    return;
  }
  Serial.println("SD card OK");
  Serial.println();
  if (! rtc.begin())
  {
    Serial.println("RTC not found!");
    while (1);
  }
  if (! rtc.isrunning())
  {
    Serial.println("RTC is not working");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  float pressure = ads.readADC_Differential_0_1();             
  Serial.print(pressure, 5);
  Serial.println(" ");
  file = SD.open("log.txt", FILE_WRITE);
  DateTime now = rtc.now();
  Serial.print(now.day() < 10 ? "0" : "");
  file.print(now.day(), DEC);
  file.print('/');
  file.print(now.month() < 10 ? "0" : "");
  file.print(now.month(), DEC);
  file.print('/');
  file.print(now.year(), DEC);
  file.print(' ');
  file.print(now.hour() < 10 ? "0" : "");
  file.print(now.hour(), DEC);
  file.print(':');
  file.print(now.minute() < 10 ? "0" : "");
  file.print(now.minute(), DEC);
  file.print(':');
  file.print(now.second() < 10 ? "0" : "");
  file.print(now.second(), DEC);
  file.print(' ');
  file.print(pressure, 5);
  file.println("");
  file.close();
  delay(500);
}
