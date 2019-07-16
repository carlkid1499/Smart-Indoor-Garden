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

bool Lights = false;
bool Water = false;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() // this code only happens once
{
  /* Code for the RTC Unit */
  Serial.begin(57600);
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  if (!rtc.initialized()) // If RTC is no running
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

  Lights = false;
  Water = false;
}

void loop()
{
  DateTime CurrTime = rtc.now(); // grabs the current time from the RTC
  /* Print out Date for troubleshooting and logs */
  Serial.print(CurrTime.year(), DEC);
  Serial.print('/');
  Serial.print(CurrTime.month(), DEC);
  Serial.print('/');
  Serial.print(CurrTime.day(), DEC);
  Serial.print(" (");
  Serial.print(CurrTime.dayOfTheWeek()); // days of week in integer form
  Serial.print(") ");
  Serial.print(CurrTime.hour(), DEC);
  Serial.print(':');
  Serial.print(CurrTime.minute(), DEC);
  Serial.print(':');
  Serial.print(CurrTime.second(), DEC);
  Serial.println();

  Serial.print("time since midnight 1/1/1970 = ");
  Serial.print(CurrTime.unixtime());
  Serial.print("s = ");
  Serial.print(CurrTime.unixtime() / 86400L);
  Serial.println("d");

  Serial.println();

  /* Check for Light Bool */
  if (Lights == true)
  {
    Serial.println("----- Message: Grow Lights If-else statement -----");
    digitalWrite(Relay_Light, HIGH);
    digitalWrite(LED_1 , HIGH);
  }
  else
    digitalWrite(Relay_Light, LOW); // if not true off
  /* End of check for Light Bool */

  /* Check for Water Bool */
  if (Water == true)
  {
    Serial.println("----- Message: Water If-else statement -----");
    digitalWrite(Relay_Water, HIGH);
    digitalWrite(LED_1 , HIGH);
  }
  else
    digitalWrite(Relay_Water, LOW); // if not true off
  /* End of check for Water Bool */
  Serial.println("----- Message: One second delay -----");
  delay(1000);
  Serial.println();

  // Let's try using some switch statements to determine water and light times
  switch (CurrTime.dayOfTheWeek())
  {
    // case for each day of the week. Turn growlights on each day at 7:01 am and water every three days

    /* ----- Sunday Schedule Begins Here ----- */
  case 0: //Sunday
    switch (CurrTime.hour())
    {
    case 7: // 7 am
      switch (CurrTime.minute())
      {
      case 1: // 7:01 am
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
        Serial.println();
        Serial.println("----- End of Message -----");
        Lights = true;
        break;

      case 30:
        switch (CurrTime.second())
        {
        case 1: // 7:30:01 am
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
          Serial.println();
          Serial.println("----- End of Message -----");
          Water = true;
          break;

        case 10: // 7:30:10 am
          Serial.println("----- Message: Water Off -----");
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
          Serial.println();
          Serial.println("----- End of Message -----");
          Water = false;
          break;
          }
      }
      break;

    case 13: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Lights = false;
        break;
      }
      break;
    }
    /* ----- Sunday Schedule Begins Ends Here ----- */

    /* ----- Monday Schedule Begins Here ----- */
  case 1:
    switch (CurrTime.hour())
    {
    case 7: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Serial.println();
        Serial.println("----- End of Message -----");
        Lights = true;
        break;
      }
      break;

    case 13: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Lights = false;
        break;
      }
      break;
    }
    /* ----- Monday Schedule Begins Ends Here ----- */

    /* ----- Tuesday Schedule Begins Here ----- */
  case 2:
    switch (CurrTime.hour())
    {
    case 7: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Serial.println();
        Serial.println("----- End of Message -----");
        Lights = true;
        break;
      }
      break;

    case 13: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Lights = false;
        break;
      }
      break;
    }
  /* ----- Tuesday Schedule Begins Ends Here ----- */

  /* ----- Wednesday Schedule Begins Here ----- */
  case 3:
    switch (CurrTime.hour())
    {
    case 7: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Serial.println();
        Serial.println("----- End of Message -----");
        Lights = true;
        break;

        case 30:
        switch (CurrTime.second())
        {
        case 1: // 7:30:01 am
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
          Serial.println();
          Serial.println("----- End of Message -----");
          Water = true;
          break;

        case 10: // 7:30:10 am
          Serial.println("----- Message: Water Off -----");
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
          Serial.println();
          Serial.println("----- End of Message -----");
          Water = false;
          break;
        }
      }
      break;

    case 13: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Lights = false;
        break;
      }
      break;
    }
  /* ----- Wednesday Schedule Begins Ends Here ----- */

  /* ----- Thursday Schedule Begins Here ----- */
  case 4:
    switch (CurrTime.hour())
    {
    case 7: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Serial.println();
        Serial.println("----- End of Message -----");
        Lights = true;
        break;
      }
      break;

    case 13: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Lights = false;
        break;
      }
      break;
    }
    /* ----- Thursday Schedule Begins Ends Here ----- */

    /* ----- Friday Schedule Begins Here ----- */
  case 5:
    switch (CurrTime.hour())
    {
    case 7: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Serial.println();
        Serial.println("----- End of Message -----");
        Lights = true;
        break;
      }
      break; // break for 7

    case 13: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Lights = false;
        break;
      }
      break; // break for 13
    }
    /* ----- Friday Schedule Begins Ends Here ----- */

    /* ----- Saturday Schedule Begins Here ----- */
  case 6:
    switch (CurrTime.hour())
    {
    case 7: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Serial.println();
        Serial.println("----- End of Message -----");
        Lights = true;
        break;
      }
      break;

    case 13: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
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
        Lights = false;
        break;
      }
      break;
    }
    /* ----- Saturday Schedule Begins Ends Here ----- */
  }
}