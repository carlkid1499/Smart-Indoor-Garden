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

  digitalWrite(Relay_Light,LOW);
  digitalWrite(Relay_Water,LOW);
}

void loop ()
{
  DateTime CurrTime = rtc.now(); // grabs the current time from the RTC

  // Let's try using some switch statements to determine water and light times
  switch (CurrTime.dayOfTheWeek())
  {
    // case for each day of the week. Turn growlights on each day and water every three days
    case 'Sunday':
      switch (CurrTime.hour())
      {
        case 7: // hour of day to do something
          digitalWrite(Relay_Light, HIGH); // lights on buddy
          Serial.println("----- Message: Grow Lights On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");

        case 8:
          digitalWrite(Relay_Water, HIGH); // water on
          Serial.println("----- Message: Water On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");
          delay(10000); // wait for 10 seconds
          digitalWrite(Relay_Water, LOW); // water off
          Serial.println("----- Message: water off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");

        case 13: // hour of day to do something
          digitalWrite(Relay_Light, LOW); // lights off buddy
          Serial.println("----- Message: Grow Lights off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");
      }
    case 'Monday':
      switch (CurrTime.hour())
      {
        case 7:
          digitalWrite(Relay_Light, HIGH);
          Serial.println("----- Message: Grow Lights On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");

        case 013: 
          digitalWrite(Relay_Light, LOW);
          Serial.println("----- Message: Grow Lights Off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----"); 
      }
    case 'Tuesday':
      switch (CurrTime.hour())
      {
        case 7: 
          digitalWrite(Relay_Light, HIGH);
          Serial.println("----- Message: Grow Lights On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");

        case 8:
          digitalWrite(Relay_Water, HIGH); // water on
          Serial.println("----- Message: Water On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");
          delay(10000); // wait for 10 seconds
          digitalWrite(Relay_Water, LOW); // water off
          Serial.println("----- Message: water off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");

        case 13:
          digitalWrite(Relay_Light, LOW);
          Serial.println("----- Message: Grow Lights Off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----"); 
      }
    case 'Wednesday':
      switch (CurrTime.hour())
      {
        case 7: 
          digitalWrite(Relay_Light, HIGH);
          Serial.println("----- Message: Grow Lights On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");

        case 13:
          digitalWrite(Relay_Light, LOW);
          Serial.println("----- Message: Grow Lights Off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");
      }
    case 'Thursday':
      switch (CurrTime.hour())
      {
        case 7: 
          digitalWrite(Relay_Light, HIGH);
          Serial.println("----- Message: Grow Lights On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");

          case 8:
          digitalWrite(Relay_Water, HIGH); // water on
          Serial.println("----- Message: Water On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");
          delay(10000); // wait for 10 seconds
          digitalWrite(Relay_Water, LOW); // water off
          Serial.println("----- Message: water off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");
          
        case 13:
          digitalWrite(Relay_Light, LOW);
          Serial.println("----- Message: Grow Lights Off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----"); 
      }
    case 'Friday':
     switch (CurrTime.hour())
      {
        case 7: 
          digitalWrite(Relay_Light, HIGH);
          Serial.println("----- Message: Grow Lights On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");

        case 13:
          digitalWrite(Relay_Light, LOW);
          Serial.println("----- Message: Grow Lights Off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");
      }
    case 'Saturday':
      switch (CurrTime.hour())
      {
        case 7: 
          digitalWrite(Relay_Light, HIGH);
          Serial.println("----- Message: Grow Lights On -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");

        case 13:
          digitalWrite(Relay_Light, LOW);
          Serial.println("----- Message: Grow Lights Off -----");
          Serial.print(CurrTime.year(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.month(), DEC);
          Serial.print('/');
          Serial.print(CurrTime.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(CurrTime.hour(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.minute(), DEC);
          Serial.print(':');
          Serial.print(CurrTime.second(), DEC);
          Serial.println("----- End of Message -----");
      }
    
  }  
}
