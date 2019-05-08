/* Smart Garden Project
   1 Relay Switch to control pumop
   RTC added on to keep track of time
   DIO 5: Rely_Light) DIO 2: Relay_Water, DIO 3: LED
*/

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

RTC_PCF8523 rtc;

// use #define to set the I/O numbers, since these will never change - this saves us memory while the Arduino is running
#define Relay_Light 5
#define Relay_Water 2
#define LED_1 3

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
DateTime now = rtc.now();
int Light_Time = now.unixtime();
int Water_Time = now.unixtime();
DateTime rebootime;

void setup () // this code only happens once
{
  /* Code for the RTC Unit */
  Serial.begin(57600);
  if (! rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) // If RTC is no running
  {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // try to adjust time anyways
  }
  else // if RTC is running
  {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  /* End of code for the RTC Unit */

  /* Code for relay switch, pump,button, and LED */
  pinMode(LED_1, OUTPUT);
  pinMode(Relay_Light, OUTPUT);
  pinMode(Relay_Water, OUTPUT);
  /* End of code for relay switch, pump,button, and LED */

  rebootime = rtc.now(); // grabs the time upon which the arduino reboots
}

void loop ()
{
  DateTime current = rtc.now();
  if (current.hour() > 7 && current.hour() < 9) /* inbetween 7 am and 9 am turn on for X hours */
  {
    digitalWrite(Relay_Light,HIGH);
  }
  else
  {
    digitalWrite(Relay_Light,LOW);
  }
  
}
