#include <RTClib.h>

/**************************************************************************
 *  This example shows how to control two relays using two pushbuttons.
 *  We will read the input from the button using digitalRead(), and based on that
 *  information we will directly set the state of the Relay using digital(write)
 *  When the button is pressed, it will give a HIGH signal, and the relay will
 *  then also go HIGH(activated)
 *
 *  Wiring for RobotGeek Sensor Shield & Sensors: 
 *    Pushbutton 1:Digital Pin 7  
 *    Pushbutton 2:Digital Pin 8 
 *    Relay 1: Digital Pin 10
 *    Relay 2: Digital Pin 11
 *    A full wiring diagram should be distributed with this code
 *    and can be found in the 'rg_dual_relay_dual_button_example' folder
 *    https://github.com/trossenrobotics/robotgeek-sensor-examples/archive/master.zip
 * 
 *  Product Links
 *    Relay Board - http://www.trossenrobotics.com/robotgeek-relay
 *    Pushbutton Board - http://www.trossenrobotics.com/robotGeek-pushbutton
 *    Sensor Shield - http://www.trossenrobotics.com/robotgeek-sensor-shield
 *    3-Pin Sensor Cables - http://www.trossenrobotics.com/robotgeek-3pin-sensor-cable
 **************************************************************************/

  // Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

 // use #define to set the I/O numbers, since these will never change - this saves us memory while the Arduino is running
#define BUTTON1 1
#define RELAY1  2

//variables to hold the current status of the button.(LOW == unpressed, HIGH = pressed)
int buttonState1 = 0;         
int buttonState2 = 0;         

void setup() {
  // set the pin for the Buzzer as output:
  pinMode(BUTTON1, INPUT);          

  // initialize the pins for the pushbutton as inputs:
  pinMode(RELAY1, OUTPUT);    

   
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
}

void loop(){
  
  //use digitalRead to store the current state of the pushbutton in one of the 'buttonState' variables
  //when pressed down, digitalRead() will read a HIGH signal. Otherwise, digitalRead will read a LOW signal
  buttonState1 = digitalRead(BUTTON1);
  
  //digital write will send whatever signal the the pushbutton currently has to the relay.
  //this means when the pushbutton is HIGH(pressed), the relay will go HIGH(Activated)
  digitalWrite(RELAY1, buttonState1); 

       DateTime now = rtc.now();
    
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
    
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    
    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));
    
    Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
    
    Serial.println();
    delay(3000);
}
