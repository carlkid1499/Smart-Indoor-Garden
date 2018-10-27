/* Smart Garden Project
 * 1 Relay Switch to control pumop
 * RTC added on to keep track of time 
 * DIO 0: LED, DIO 1: Push Button DIO 2: Relay
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
#define LED_1    0
#define BUTTON_1 1
#define RELAY_1  2

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
DateTime future;
DateTime stopwater;
bool OnOff;s

void setup () 
{
  // LED
  #define LED_1 0
  pinMode(LED_1, OUTPUT);
  /* Code for relay switch, pump, and button */
  pinMode(BUTTON_1, INPUT);          
  pinMode(RELAY_1, OUTPUT);   
  /* End of code for relay switch, pump, and button */ 

  /* Code for the RTC Unit */
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif

  Serial.begin(57600);
  if (! rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) 
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  /* End of code for the RTC Unit */

  future = Get_Future_Time();
  stopwater = (future + TimeSpan(0,0,0,5));
}

void loop () 
{
  Check_Water();
}

DateTime Get_Future_Time() //  Funtion to grab future time 3 days
{
    DateTime now = rtc.now();
    DateTime future (now + TimeSpan(0,0,1,0));
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
    return future;
}

void Check_Water()
{
  DateTime now = rtc.now();
  /* Begin code for Relay */
  if( (now.year() == future.year()) && (now.month() == future.month()) &&
  (now.day() == future.day()) && (now.hour() == future.hour()) && 
  (now.minute() == future.minute()) && (now.second() == future.second()) )
  {
  Water_On();
  }
  if((now.year() == stopwater.year()) && (now.month() == stopwater.month()) &&
    (now.day() == stopwater.day()) && (now.hour() == stopwater.hour()) && 
    (now.minute() == stopwater.minute()) && (now.second() == stopwater.second()))
  {
  Water_Off();
  }
  /* End code for Relay */ 
}

void Water_On() // check current time against future time
{
    Serial.println();
    Serial.println("Message: Water pump on");
    digitalWrite(RELAY_1,1);
}

void Water_Off() // checks current time against stop time
{
    digitalWrite(RELAY_1,0);
    Serial.println("Message: Water pump off");
    Serial.println("Message: Setting new future time");
    future = Get_Future_Time();
    Serial.println();
    Serial.println("Message: Seting new stopwater time");
    stopwater = (future + TimeSpan(0,0,0,5));
}
