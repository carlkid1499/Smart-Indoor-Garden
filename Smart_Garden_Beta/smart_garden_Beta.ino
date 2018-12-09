/* Smart Garden Project
 * 1 Relay Switch to control pumop
 * RTC added on to keep track of time 
 * DIO 1: Push Button DIO 2: Relay, DIO 3: LED 
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
//bool OnOff = false;

void setup () 
{
  /* Code for the RTC Unit */
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

  future = Get_Future_Time(); // grab future data
  stopwater = (future + TimeSpan(0,0,0,15)); // set water time

  /* Code for relay switch, pump,button, and LED */
  pinMode(LED_1, OUTPUT);
  pinMode(BUTTON_1, INPUT);          
  pinMode(RELAY_1, OUTPUT);   
  /* End of code for relay switch, pump,button, and LED */ 

}

void loop () 
{
  Water_On();
  Water_Off();
}

DateTime Get_Future_Time() //  Funtion to grab future time 3 days
{
    DateTime now = rtc.now();
    DateTime future (now + TimeSpan(0,0,0,30)); // 3 days
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

void Water_On() // check current time against future time
{
  DateTime now = rtc.now();
  if( (now.year() == future.year()) && (now.month() == future.month()) &&
  (now.day() == future.day()) && (now.hour() == future.hour()) && 
  (now.minute() == future.minute()) && (now.second() == future.second()) )
  {
    digitalWrite(RELAY_1,1);
    Serial.println("Message: Water pump on");
    //OnOff = true;
    //Water_On_LED();
  }
}

void Water_Off() // checks current time against stop time
{
  DateTime now = rtc.now();
  if((now.year() == stopwater.year()) && (now.month() == stopwater.month()) &&
    (now.day() == stopwater.day()) && (now.hour() == stopwater.hour()) && 
    (now.minute() == stopwater.minute()) && (now.second() == stopwater.second()))
  {
    digitalWrite(RELAY_1,0);
    Serial.println("Message: Water pump off");
    Serial.println("Message: Setting new future time");
    future = Get_Future_Time();
    Serial.println("Message: Seting new stopwater time");
    stopwater = (future + TimeSpan(0,0,0,15));
    Serial.print(stopwater.year(), DEC);
    Serial.print('/');
    Serial.print(stopwater.month(), DEC);
    Serial.print('/');
    Serial.print(stopwater.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[future.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(stopwater.hour(), DEC);
    Serial.print(':');
    Serial.print(stopwater.minute(), DEC);
    Serial.print(':');
    Serial.print(stopwater.second(), DEC);
    Serial.println();
    //OnOff = false;
  }
}

void Water_On_LED()
{
    // blink for water on
    digitalWrite(LED_1, HIGH);
    delay(100);                      
    digitalWrite(LED_1, LOW);
    delay(100);
    digitalWrite(LED_1, HIGH);
    delay(100);                    
    digitalWrite(LED_1, LOW); 
    delay(100);                       
    digitalWrite(LED_1, HIGH);  
    delay(1000);                       
    digitalWrite(LED_1, LOW);  
    delay(1000);
}
