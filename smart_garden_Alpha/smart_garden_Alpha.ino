/* Smart Garden Project */
/* 1 Relay Switch to control pumop */
/* RTC added on to keep track of time */

// use #define to set the I/O numbers, since these will never change - this saves us memory while the Arduino is running
#define BUTTON1 1
#define RELAY1  2

//variables to hold the current status of the button.(LOW == unpressed, HIGH = pressed)
int buttonState1 = 0;         
int buttonState2 = 0;

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
DateTime Get_Future_Time();

void setup () 
{
/* Code for relay switch and pump */

   // set the pin for the Buzzer as output:
  pinMode(BUTTON1, INPUT);          

  // initialize the pins for the pushbutton as inputs:
  pinMode(RELAY1, OUTPUT);   
/* End of code for relay switch and pump */ 

/* Code for the RTC Unit */
#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
/* End of code for the RTC Unit */
}

void loop () 
{
/* Code for the RTC Unit */
    DateTime now = rtc.now(); // grabs current time
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Get_Future_Time();
    Serial.print(" now + 3d ");
    Serial.print(Get_Future_Time().year(), DEC);
    Serial.print('/');
    Serial.print(Get_Future_Time().month(), DEC);
    Serial.print('/');
    Serial.print(Get_Future_Time().day(), DEC);
    Serial.print(' ');
    Serial.print(Get_Future_Time().hour(), DEC);
    Serial.print(':');
    Serial.print(Get_Future_Time().minute(), DEC);
    Serial.print(':');
    Serial.print(Get_Future_Time().second(), DEC);
    Serial.println();
    Serial.println();
    delay(3000);
/* End of code for the RTC Unit */
}

DateTime Get_Future_Time() //  Funtion to grab future time 3 days
{
    DateTime now = rtc.now();
    DateTime future (now + TimeSpan(3,0,0,0));
    return future;
}
