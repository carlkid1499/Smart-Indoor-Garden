/* Smart Garden Project
   1 Relay Switch to control pumop
   RTC added on to keep track of time
   DIO 5: Realy 1 DIO 2: Relay 2, DIO 3: LED
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
#define RELAY_1 5
#define RELAY_2 2
#define LED_1 3

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
DateTime futurewater;
DateTime futurelight;
DateTime stopwater;
DateTime stoplight;

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

  futurewater = Get_futurewater_Time(); // grab futurewater
  futurelight = Get_futurelight_Time(); // grab futurelight
  /* Code for relay switch, pump,button, and LED */
  pinMode(LED_1, OUTPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  /* End of code for relay switch, pump,button, and LED */

}

DateTime Get_futurewater_Time() //  Funtion to grab futurewater time 3 days
{
  DateTime now = rtc.now();
  DateTime futurewater (now + TimeSpan(3, 12, 0, 0)); // 3 days
  Serial.println("Message: futurewater time grabbed");
  Serial.print(futurewater.year(), DEC);
  Serial.print('/');
  Serial.print(futurewater.month(), DEC);
  Serial.print('/');
  Serial.print(futurewater.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[futurewater.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(futurewater.hour(), DEC);
  Serial.print(':');
  Serial.print(futurewater.minute(), DEC);
  Serial.print(':');
  Serial.print(futurewater.second(), DEC);
  Serial.println();
  return futurewater;
}

DateTime Get_futurelight_Time() //  Funtion to grab futurewater time 1 days
{
  DateTime now = rtc.now();
  DateTime futurelight (now + TimeSpan(1, 8, 0, 0)); // 3 days 
  Serial.println("Message: futurelight time grabbed");
  Serial.print(futurelight.year(), DEC);
  Serial.print('/');
  Serial.print(futurelight.month(), DEC);
  Serial.print('/');
  Serial.print(futurelight.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[futurewater.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(futurelight.hour(), DEC);
  Serial.print(':');
  Serial.print(futurelight.minute(), DEC);
  Serial.print(':');
  Serial.print(futurelight.second(), DEC);
  Serial.println();
  return futurelight;
}

void loop ()
{
  DateTime now = rtc.now();

  // check time for lighting
  if ( ( now.day() == futurewater.day()) && (now.hour() == futurewater.hour()) )
  {
    digitalWrite(RELAY_1, HIGH);
  }
  else if ( ( ( now.day() == futurewater.day()) && (now.hour() == futurewater.hour() + 4) ))
  {
    digitalWrite(RELAY_1, LOW);
    futurelight = Get_futurelight_Time();
  }
  else
  {
    digitalWrite(RELAY_1, LOW);
  }


  
  // check time for watering
  if ( (now.year() == futurewater.year()) && (now.month() == futurewater.month()) &&
       (now.day() == futurewater.day()) && (now.hour() == futurewater.hour()) &&
       (now.minute() == futurewater.minute()) && (now.second() == futurewater.second()) )
  {
    digitalWrite(RELAY_2, HIGH);
    Serial.println("Message: Water pump on");
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
    // blink for water on
    int numblink = 0;
    while (numblink < 10) // each blink is 1 second
    {
      digitalWrite(LED_1, HIGH);
      delay(500);
      digitalWrite(LED_1, LOW);
      delay(500);
      numblink++;
    }
    DateTime now = rtc.now(); // grab time again
    Serial.println();
    Serial.println("Message: Water pump off");
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
    Serial.println("Message: Setting new futurewater time");
    futurewater = Get_futurewater_Time(); // set new watering time
    Serial.println();
  }
  else // else always keep it off
  {
    digitalWrite(RELAY_2, LOW);
    digitalWrite(LED_1, HIGH);
    delay(150);
    digitalWrite(LED_1, LOW);
    delay(150);
  }
}
