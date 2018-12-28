/* Smart Garden Project
   1 Relay Switch to control pumop
   RTC added on to keep track of time
   DIO 1: Push Button DIO 2: Relay, DIO 3: LED
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
#define BUTTON_1 1
#define RELAY_1  2
#define LED_1 3

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
DateTime future;
DateTime stopwater;

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

  future = Get_Future_Time(); // grab future data

  /* Code for relay switch, pump,button, and LED */
  pinMode(LED_1, OUTPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(RELAY_1, OUTPUT);
  /* End of code for relay switch, pump,button, and LED */

}

DateTime Get_Future_Time() //  Funtion to grab future time 3 days
{
  DateTime now = rtc.now();
  DateTime future (now + TimeSpan(3, 8, 0, 0)); // 3 days
  Serial.println("Message: Future time grabbed");
  Serial.print(future.year(), DEC);
  Serial.print('/');
  Serial.print(future.month(), DEC);
  Serial.print('/');
  Serial.print(future.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[future.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(future.hour(), DEC);
  Serial.print(':');
  Serial.print(future.minute(), DEC);
  Serial.print(':');
  Serial.print(future.second(), DEC);
  Serial.println();
  return future;
}

void loop ()
{
  DateTime now = rtc.now();
  // check time for watering
  if ( (now.year() == future.year()) && (now.month() == future.month()) &&
       (now.day() == future.day()) && (now.hour() == future.hour()) &&
       (now.minute() == future.minute()) && (now.second() == future.second()) )
  {
    digitalWrite(RELAY_1, HIGH);
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
    while (numblink < 20) // each blink is 1 second
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
    Serial.println("Message: Setting new future time");
    future = Get_Future_Time(); // set new watering time
    Serial.println();
  }
  else // else always keep it off
  {
    digitalWrite(RELAY_1, LOW);
    digitalWrite(LED_1, HIGH);
    delay(150);
    digitalWrite(LED_1, LOW);
    delay(150);
  }
}
