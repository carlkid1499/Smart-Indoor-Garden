/* Smart Garden Project
   1 Relay Switch to control pumop
   RTC added on to keep track of time and SD
   DIO 5: Rely_Light DIO 2: Relay_Water_1, DIO 3: LED,  DIO 6: Relay_Water_2
   DIO 10: SD CardSelect, DIO 11: MOSI,  DIO 12: MISO,  DIO 13: SCLK
   AIO-0: Photocell
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
#define photocellPin 0 // the cell and 10K pulldown are connected to a0
int photocellReading; // the analog reading from the analog resistor divider

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
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i = errno; i < 10; i++)
    {
      delay(200);
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

  uint8_t i = 0;
  /* ----- End: Setup code for SD Card ----- */

  DateTime PowerTime = rtc.now();
  /* ----- BEGIN: Initial Message. Power On. ----- */
  logfile.println("----- Message: System  Power on -----");
  logfile.print(PowerTime.year(), DEC);
  logfile.print('/');
  logfile.print(PowerTime.month(), DEC);
  logfile.print('/');
  logfile.print(PowerTime.day(), DEC);
  logfile.print(" (");
  logfile.print(daysOfTheWeek[PowerTime.dayOfTheWeek()]);
  logfile.print(") ");
  logfile.print(PowerTime.hour(), DEC);
  logfile.print(':');
  logfile.print(PowerTime.minute(), DEC);
  logfile.print(':');
  logfile.print(PowerTime.second(), DEC);
  logfile.println();
  logfile.println("----- End of Message -----");
  logfile.flush();
}

void loop()
{

  DateTime CurrTime = rtc.now(); // grabs the current time from the RTC
  // Check the PhotoCell Pin every time it changes
  photocellReading = analogRead(photocellPin);

  logfile.println("----- Message: PhotoCell Reading -----");
  logfile.print(photocellReading); // the raw analog reading

  // We'll have a few threshholds, qualitatively determined
  if (photocellReading < 10)
  {
    logfile.println(" - Dark");
    logfile.println("----- End of Message -----");
  }
  else if (photocellReading < 200)
  {
    logfile.println(" - Dim");
    logfile.println("----- End of Message -----");
  }
  else if (photocellReading < 500)
  {
    logfile.println(" - Light");
    logfile.println("----- End of Message -----");
  }
  else if (photocellReading < 800)
  {
    logfile.println(" - Bright");
    logfile.println("----- End of Message -----");
  }
  else
  {
    logfile.println(" - Very bright");
    logfile.println("----- End of Message -----");
  }
  logfile.flush(); 

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
              logfile.println("----- Message: Grow Lights On -----");
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
              digitalWrite(Relay_Light, HIGH);
              logfile.flush();
              break; // Sunday Case 1 break
            case 30:
              switch (CurrTime.second())
              {
                case 1: // 7:30:01 am
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
                  delay(20000); //delay 20 secoonds
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

                case 30: // 7:30:30 am
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
                  delay(20000); //delay 20 secoonds
                  CurrTime = rtc.now();
                  digitalWrite(Relay_Water_2, LOW);
                  logfile.println("----- Message: Water  Pump 2 Off -----");
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
              }
              break; // Case 30 Break
          }
          break;

        case 13: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights Off -----");
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
              digitalWrite(Relay_Light, LOW);
              logfile.flush();
              break;
          }
          break;
      }
      break;
    /* ----- Sunday Schedule Begins Ends Here ----- */

    /* ----- Monday Schedule Begins Here ----- */
    case 1:
      switch (CurrTime.hour())
      {
        case 7: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights On -----");
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
              digitalWrite(Relay_Light, HIGH);
              logfile.flush();
              break;
          }
          break;

        case 13: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights Off -----");
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
              digitalWrite(Relay_Light, LOW);
              logfile.flush();
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
        case 7: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights On -----");
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
              digitalWrite(Relay_Light, HIGH);
              logfile.flush();
              break;
          }
          break;

        case 13: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights Off -----");
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
              digitalWrite(Relay_Light, LOW);
              logfile.flush();
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
        case 7: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights On -----");
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
              digitalWrite(Relay_Light, HIGH);
              logfile.flush();
              break;

            case 30:
              switch (CurrTime.second())
              {
                case 1: // 7:30:01 am
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
                  delay(10000); //delay 10 secoonds
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

                case 30: // 7:30:30 am
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
                  delay(10000); //delay 10 secoonds
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
              }
          }
          break;

        case 13: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights Off -----");
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
              digitalWrite(Relay_Light, LOW);
              logfile.flush();
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
        case 7: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights On -----");
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
              digitalWrite(Relay_Light, HIGH);
              logfile.flush();
              break;
          }
          break;

        case 13: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights Off -----");
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
              digitalWrite(Relay_Light, LOW);
              logfile.flush();
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
        case 7: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights On -----");
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
              digitalWrite(Relay_Light, HIGH);
              logfile.flush();
              break;
          }
          break; // break for 7

        case 13: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights Off -----");
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
              digitalWrite(Relay_Light, LOW);
              logfile.flush();
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
        case 7: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights On -----");
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
              digitalWrite(Relay_Light, HIGH);
              logfile.flush();
              break;
          }
          break;

        case 13: // hour of day to do something
          switch (CurrTime.minute())
          {
            case 1:
              logfile.println("----- Message: Grow Lights Off -----");
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
              digitalWrite(Relay_Light, LOW);
              logfile.flush();
              break;
          }
          break;
      }
      break;
      /* ----- Saturday Schedule Begins Ends Here ----- */
  }
}
