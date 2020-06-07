/* Smart Garden Project
** 2 Relay switches to control 2 pumps
** RTC added on to keep track of time and SD
** DIO 5: Rely_Light DIO 2: Relay_Water_1, DIO 3: LED,  DIO 6: Relay_Water_2
** DIO 10: SD CardSelect, DIO 11: MOSI,  DIO 12: MISO,  DIO 13: SCLK
** AIO-0: Photocell
*/

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
// Library for SD card on RTC
#include <SPI.h>
#include <SD.h>

RTC_PCF8523 rtc;

// use #define to set the I/O numbers, since these will never change - this saves us memory while the Arduino is running
#define Relay_Light 5
#define Relay_Water_1 2
#define Relay_Water_2 6
#define LED_1 3
#define cardSelect 10
char *message;         // Dynamic array memory for log messages

// Name of the log file. It'll be on boot up.
File logfile;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// blink out an error code
void error(uint8_t errno)
{
  while (1)
  {
    uint8_t i;
    for (i = 0; i < errno; i++)
    {
      digitalWrite(13, HIGH);
      digitalWrite(LED_1, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      digitalWrite(LED_1, LOW);
      delay(100);
    }
    for (i = errno; i < 10; i++)
    {
      delay(2000);
    }
  }
}

void setup() // this code only happens once
{
  /* Code for the RTC Unit */
  Serial.begin(57600);
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    error(10);
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
  pinMode(Relay_Water_1, OUTPUT);
  pinMode(Relay_Water_2, OUTPUT);
  /* End of code for relay switch, pump,button, and LED */

  /* Set both relays to LOW upon power reset/set up */
  digitalWrite(Relay_Light, LOW);
  digitalWrite(Relay_Water_1, LOW);
  digitalWrite(Relay_Water_2, LOW);

  /* End: Set both relays to LOW upon power reset/set up */

  /* ----- Begin: Setup code for SD Card ----- */
  Serial.println("\r\nAnalog logger test");
  pinMode(13, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect))
  {
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "/DATLOG00.TXT");
  for (uint8_t i = 0; i < 100; i++)
  {
    filename[7] = '0' + i / 10;
    filename[8] = '0' + i % 10;
    // create if does not exist, do not open existing, write, sync after write
    if (!SD.exists(filename))
    {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if (!logfile)
  {
    Serial.print("Couldnt create ");
    Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to ");
  Serial.println(filename);

  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("Ready!");

  /* ----- End: Setup code for SD Card ----- */

  DateTime PowerTime = rtc.now();
  /* ----- BEGIN: Initial Message. Power On. ----- */
  new char message = "----- Message: System Power On -----";
  log_msg(message, PowerTime);
  delete[] message;
}

void loop()
{

  DateTime CurrTime = rtc.now(); // grabs the current time from the RTC
  switch (CurrTime.dayOfTheWeek())
  {
  /* ----- Sunday Schedule Begins Here ----- */
  case 0: //Sunday
    switch (CurrTime.hour())
    {
    case 8: // 8 am
      switch (CurrTime.minute())
      {
      case 1: // 8:01 am
      {
        new char message = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        delete[] message;
        digitalWrite(Relay_Light, HIGH);
        break; // Sunday Case 1 break
      }
      case 30:
        switch (CurrTime.second())
        {
        case 1: // 8:30:01 am
        {
          new char message = "----- Message: Water Pump 1 On -----";
          log_msg(message, CurrTime);
          delete[] message;
          digitalWrite(Relay_Water_1, HIGH);
          delay(20000); //delay 20 secoonds
          CurrTime = rtc.now();
          digitalWrite(Relay_Water_1, LOW);
          message = "----- Message: Water Pump 1 Off -----";
          log_msg(message, CurrTime);
          delete[] message;
          break;
        }
        case 31: // 8:30:31 am
        {
          message = "----- Message: Water Pump 2 On -----";
          log_msg(message, CurrTime);
          delete[] message;
          digitalWrite(Relay_Water_2, HIGH);
          delay(25000); //delay 20 secoonds
          CurrTime = rtc.now();
          digitalWrite(Relay_Water_2, LOW);
          message = "----- Message: Water Pump 2 Off -----";
          log_msg(message, CurrTime);
          delete[] message;
          break;
        }
        }
        break; // Case 30 Break
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
      {
        message = "----- Message: Grow Lights Off -----";
        log_msg(message, CurrTime);
        delete[] message;
        digitalWrite(Relay_Light, LOW);
        break;
      }
      }
      break;
    }
    break;
  /* ----- Sunday Schedule Begins Ends Here ----- */

  /* ----- Monday Schedule Begins Here ----- */
  case 1:
    switch (CurrTime.hour())
    {
    case 8: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        delete[] message;
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights Off -----";
        log_msg(message, CurrTime);
        delete[] message;
        digitalWrite(Relay_Light, LOW);
        break;
      }
      break;
    }
    break;
  /* ----- Monday Schedule Begins Ends Here ----- */

  /* ----- Tuesday Schedule Begins Here ----- */
  case 2:
    switch (CurrTime.hour())
    {
    case 8: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        delete[] message;
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights Off -----";
        log_msg(message, CurrTime);
        digitalWrite(Relay_Light, LOW);
        break;
      }
      break;
    }
    break;
  /* ----- Tuesday Schedule Begins Ends Here ----- */

  /* ----- Wednesday Schedule Begins Here ----- */
  case 3:
    switch (CurrTime.hour())
    {
    case 8: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        digitalWrite(Relay_Light, HIGH);
        break;

        /* case 30:
              switch (CurrTime.second())
              {
                case 1: // 8:30:01 am
                  logfile.println("----- Message: Water Pump 1 On -----");
                  logfile.print(CurrTime.year(), DEC);
                  logfile.print('/');
                  logfile.print(CurrTime.month(), DEC);
                  logfile.print('/');
                  logfile.print(CurrTime.day(), DEC);
                  logfile.print(" (");
                  logfile.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
                  logfile.print(") ");
                  logfile.print(CurrTime.hour(), DEC);
                  logfile.print(':');
                  logfile.print(CurrTime.minute(), DEC);
                  logfile.print(':');
                  logfile.print(CurrTime.second(), DEC);
                  logfile.println();
                  logfile.println("----- End of Message -----");
                  digitalWrite(Relay_Water_1, HIGH);
                  delay(15000); //delay 15 secoonds
                  CurrTime = rtc.now();
                  digitalWrite(Relay_Water_1, LOW);
                  logfile.println("----- Message: Water Pump 1 Off -----");
                  logfile.print(CurrTime.year(), DEC);
                  logfile.print('/');
                  logfile.print(CurrTime.month(), DEC);
                  logfile.print('/');
                  logfile.print(CurrTime.day(), DEC);
                  logfile.print(" (");
                  logfile.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
                  logfile.print(") ");
                  logfile.print(CurrTime.hour(), DEC);
                  logfile.print(':');
                  logfile.print(CurrTime.minute(), DEC);
                  logfile.print(':');
                  logfile.print(CurrTime.second(), DEC);
                  logfile.println();
                  logfile.println("----- End of Message -----");
                  logfile.flush();
                  break;

                case 30: // 8:30:30 am
                  logfile.println("----- Message: Water Pump 2 On -----");
                  logfile.print(CurrTime.year(), DEC);
                  logfile.print('/');
                  logfile.print(CurrTime.month(), DEC);
                  logfile.print('/');
                  logfile.print(CurrTime.day(), DEC);
                  logfile.print(" (");
                  logfile.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
                  logfile.print(") ");
                  logfile.print(CurrTime.hour(), DEC);
                  logfile.print(':');
                  logfile.print(CurrTime.minute(), DEC);
                  logfile.print(':');
                  logfile.print(CurrTime.second(), DEC);
                  logfile.println();
                  logfile.println("----- End of Message -----");
                  digitalWrite(Relay_Water_2, HIGH);
                  delay(15000); //delay 15 secoonds
                  CurrTime = rtc.now();
                  digitalWrite(Relay_Water_2, LOW);
                  logfile.println("----- Message: Water Pump 2 Off -----");
                  logfile.print(CurrTime.year(), DEC);
                  logfile.print('/');
                  logfile.print(CurrTime.month(), DEC);
                  logfile.print('/');
                  logfile.print(CurrTime.day(), DEC);
                  logfile.print(" (");
                  logfile.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
                  logfile.print(") ");
                  logfile.print(CurrTime.hour(), DEC);
                  logfile.print(':');
                  logfile.print(CurrTime.minute(), DEC);
                  logfile.print(':');
                  logfile.print(CurrTime.second(), DEC);
                  logfile.println();
                  logfile.println("----- End of Message -----");
                  logfile.flush();
                  break;
              } */
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights Off -----";
        log_msg(message, CurrTime);
        digitalWrite(Relay_Light, LOW);
        break;
      }
      break;
    }
    break;
  /* ----- Wednesday Schedule Begins Ends Here ----- */

  /* ----- Thursday Schedule Begins Here ----- */
  case 4:
    switch (CurrTime.hour())
    {
    case 8: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights Off -----";
        log_msg(message, CurrTime);
        digitalWrite(Relay_Light, LOW);
        break;
      }
      break;
    }
    break;
  /* ----- Thursday Schedule Begins Ends Here ----- */

  /* ----- Friday Schedule Begins Here ----- */
  case 5:
    switch (CurrTime.hour())
    {
    case 8: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break; // break for 7

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights Off -----";
        log_msg(message, CurrTime);
        digitalWrite(Relay_Light, LOW);
        break;
      }
      break; // break for 13
    }
    break;
  /* ----- Friday Schedule Begins Ends Here ----- */

  /* ----- Saturday Schedule Begins Here ----- */
  case 6:
    switch (CurrTime.hour())
    {
    case 8: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        message = "----- Message: Grow Lights Off -----";
        log_msg(message, CurrTime);
        digitalWrite(Relay_Light, LOW);
        break;
      }
      break;
    }
    break;
    /* ----- Saturday Schedule Begins Ends Here ----- */
  }
}

/* Function for logging to SD card */
void log_msg(char *message, DateTime CurrTime)
{
  logfile.println(message);
  logfile.print(CurrTime.year(), DEC);
  logfile.print('/');
  logfile.print(CurrTime.month(), DEC);
  logfile.print('/');
  logfile.print(CurrTime.day(), DEC);
  logfile.print(" (");
  logfile.print(daysOfTheWeek[CurrTime.dayOfTheWeek()]);
  logfile.print(") ");
  logfile.print(CurrTime.hour(), DEC);
  logfile.print(':');
  logfile.print(CurrTime.minute(), DEC);
  logfile.print(':');
  logfile.print(CurrTime.second(), DEC);
  logfile.println("----- End of Message -----");
  logfile.flush();
}
