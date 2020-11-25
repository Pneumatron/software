
/* Copyright (c) 2020 - Trabi et al.
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/  

/////////////////////////////////////////////////////////////////////////////////////////////////
// Pneumatron Software - semi-automated mode - v1 (10/2020)
// Parameters to be adjusted:
// 1- Calibration: Define slope and intercept from calibration:
float slope = 0.0183;
float intercept = 0.297;
// 2- Set time of gas discharge measurement, in seconds * 0.5 (e.g. 120= 60 seconds):
int duration = 120;
// 3- Set pressure range, in kPa
int PressureHigh = 50;
int PressureLow = 10;
//
/////////////////////////////////////////////////////////////////////////////////////////////////

  
int Pump = 6;
int Solenoid = 5;

const int buttonPin = 7;
int buttonState = 0; 

long time_1 = 1;
long time_2 = 1;
long measure = 1;

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
  digitalWrite(Pump, LOW);
  digitalWrite(Solenoid, LOW);
   
    Serial.println("Data logger module");
    Serial.println();

  Serial.println("Starting SD...");
  if (!SD.begin(Pino_CS))
  {
  Serial.println("error to start SD!");
    return;
  }
  Serial.println("SD card OK");
  Serial.println();

  //Verifica as condicoes do RTC
  if (! rtc.begin())
  {
    Serial.println("RTC not found!");
    while (1);
  }
  if (! rtc.isrunning())
  {
    Serial.println("RTC is not working");
    
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //Manual adjustment of date and time:
    //Formate: DateTime(year, month, day, hour, min, sec)
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //automated adjustment of data and time when you upload the program:
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}


void loop() {
      buttonState = digitalRead(buttonPin);
      if (buttonState == HIGH)
      {
      for(int i = 0; i < duration; i++) 
      { 
     float pressure = ads.readADC_Differential_0_1();
    pressure = ((pressure)*(slope)+intercept);
             
     Serial.print(pressure, 5);
     Serial.println(" ");
              
        if (pressure > PressureHigh)
        {
        digitalWrite(Pump, LOW);
        digitalWrite(Solenoid, LOW);  
        }
        else if (pressure < PressureLow)
        {
        digitalWrite(Pump, HIGH);
        digitalWrite(Solenoid, HIGH);
        }
  file = SD.open("log.csv", FILE_WRITE);
  DateTime now = rtc.now();
  file.print(now.day() < 10 ? "0" : "");
  file.print(now.day(), DEC);
  file.print('/');
  file.print(now.month() < 10 ? "0" : "");
  file.print(now.month(), DEC);
  file.print('/');
  file.print(now.year(), DEC);
  file.print(',');
  file.print(now.hour() < 10 ? "0" : "");
  file.print(now.hour(), DEC);
  file.print(':');
  file.print(now.minute() < 10 ? "0" : "");
  file.print(now.minute(), DEC);
  file.print(':');
  file.print(now.second() < 10 ? "0" : "");
  file.print(now.second(), DEC);
  file.print(',');
  file.print(time_1);
  file.print(',');
  file.print(measure);
  file.print(',');
  file.print(time_2);
  file.print(',');
  file.print(pressure, 5);
  file.println("");
  file.close();   
        delay(500);
        time_1++;
        time_2++;
    }
      digitalWrite(Pump, LOW);
      measure++;
      time_2 = 1;
    }
}
