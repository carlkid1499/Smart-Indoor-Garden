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
void Water_On();
void Water_Off();
DateTime future;
DateTime stopwater;
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
  }
/* End of code for the RTC Unit */

future = Get_Future_Time();
stopwater = (future + TimeSpan(0,0,0,10));
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
    Serial.println();
    delay(1000);
/* End of code for the RTC Unit */
/* Begin code for Relay */
  Water_On();
  Water_Off();
/* End code for Relay */

}

DateTime Get_Future_Time() //  Funtion to grab future time 3 days
{
    DateTime now = rtc.now();
    DateTime future (now + TimeSpan(0,0,0,10));
    Serial.println("Message: Future time grabbed");
    
    return future;
}

void Water_On() // check current time against future time
{
  DateTime now = rtc.now();
  if( (now.year() == future.year()) && (now.month() == future.month()) &&
  (now.day() == future.day()) && (now.hour() == future.hour()) && 
  (now.minute() == future.minute()) && (now.second() == future.second()) )
  {
    digitalWrite(RELAY1,1);
    Serial.println("Message: Water pump on");
  }
}

void Water_Off() // checks current time against stop time
{
  DateTime now = rtc.now();
  if((now.year() == stopwater.year()) && (now.month() == stopwater.month()) &&
    (now.day() == stopwater.day()) && (now.hour() == stopwater.hour()) && 
    (now.minute() == stopwater.minute()) && (now.second() == stopwater.second()))
  {
    digitalWrite(RELAY1,0);
    Serial.println("Message: Water pump off");
    Serial.println("Message: Setting new future time");
    future = Get_Future_Time();
   
    Serial.println("Message: Seting new stopwater time");
    stopwater = (future + TimeSpan(0,0,0,10));
  
  }
}
