/* Smart Garden Project
** https://github.com/carlkid1499/Smart-Garden  
** 2 Relay switches to control 2 pumps
** RTC added on to keep track of time and SD
** DIO 5: Rely_Water_1
** DIO 6: Relay_Water_2
** DIO 7: Relay_Light
** DIO 8: LED_1
** DIO 9: ESP CS
** DIO 10: SD CS
** DIO 11: MOSI
** DIO 12: MISO 
** DIO 13: SCLK
*/

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
// Libs for SD card on RTC
#include <SPI.h>
#include <SD.h>
// Libs for ESP

RTC_PCF8523 rtc;

#define Relay_Light 7
#define Relay_Water_1 5
#define Relay_Water_2 6
#define LED_1 8
#define ESPCS 9
#define SDCS 10

// Name of the log file. It'll be on boot up.
File logfile;

// Create 2D array for days
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void printDirectory(File dir, int numTabs);
void error(uint8_t errno);
void setup();
void loop();
void log_msg(char *message, DateTime CurrTime);

// Define error function
void error(uint8_t errno)
{
  while (1)
  {
    uint8_t i;
    for (i = 0; i < errno; i++)
    {
      digitalWrite(LED_1, HIGH);
      delay(100);
      digitalWrite(LED_1, LOW);
      delay(100);
    }
    for (i = errno; i < 10; i++)
    {
      delay(2000);
    }
  }
}

// define the setup function
void setup()
{
  /***** BEGIN: Code for the RTC Unit *****/
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
  /***** END: Code for the RTC Unit *****/

  /***** BEGIN: pinModes *****/
  pinMode(LED_1, OUTPUT);
  pinMode(Relay_Light, OUTPUT);
  pinMode(Relay_Water_1, OUTPUT);
  pinMode(Relay_Water_2, OUTPUT);
  pinMode(ESPCS, OUTPUT);
  pinMode(SDCS, OUTPUT);
  /***** END: pinModes *****/

  /***** BEGIN: Init Relays *****/
  digitalWrite(Relay_Light, LOW);
  digitalWrite(Relay_Water_1, LOW);
  digitalWrite(Relay_Water_2, LOW);
  /***** END: Init Relays *****/

  /***** Begin: Setup code for SD Card *****/
  Serial.println("\r\nAnalog logger test");

  // see if the card is present and can be initialized:
  if (!SD.begin(SDCS))
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
  Serial.println("Ready!");
  /***** END: Setup code for SD Card *****/

  /***** BEGIN: Initial Message. Power On. *****/
  DateTime PowerTime = rtc.now();
  char message [] = "----- Message: System Power On -----";
  log_msg(message, PowerTime);
  /***** END: Initial Message. Power On. *****/
  
  /***** If we get here success *****/
  digitalWrite(LED_1, HIGH); 
}

// define the infinite loop function
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
        
        char message [] = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        
        digitalWrite(Relay_Light, HIGH);
        break; // Sunday Case 1 break
      }
      case 30:
        switch (CurrTime.second())
        {
        case 1: // 8:30:01 am
        {
          
          char message [] = "----- Message: Water Pump 1 On -----";
          log_msg(message, CurrTime);
          
          digitalWrite(Relay_Water_1, HIGH);
          delay(20000); //delay 25 secoonds
          CurrTime = rtc.now();
          digitalWrite(Relay_Water_1, LOW);
          
          char message2 [] = "----- Message: Water Pump 1 Off -----";
          log_msg(message2, CurrTime);
          
          break;
        }
        case 31: // 8:30:31 am
        {
          
          char message [] = "----- Message: Water Pump 2 On -----";
          log_msg(message, CurrTime);
          
          digitalWrite(Relay_Water_2, HIGH);
          delay(25000); //delay 20 secoonds
          CurrTime = rtc.now();
          digitalWrite(Relay_Water_2, LOW);
          
          char message2 [] = "----- Message: Water Pump 2 Off -----";
          log_msg(message2, CurrTime);
          
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
        
        char message [] = "----- Message: Grow Lights Off -----";
        log_msg(message, CurrTime);
        
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
        
        char message [] = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        
        char message [] = "----- Message: Grow Lights Off -----";
        log_msg(message, CurrTime);
        
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
        
        char message [] = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        
        char message [] = "----- Message: Grow Lights Off -----";
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
        
        char message [] = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        
        char message [] = "----- Message: Grow Lights Off -----";
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
        
        char message [] = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
         
        char message [] = "----- Message: Grow Lights Off -----";
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
        
        char message [] = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break; // break for 7

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        
        char message [] = "----- Message: Grow Lights Off -----";
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
        
        char message [] = "----- Message: Grow Lights On -----";
        log_msg(message, CurrTime);
        
        digitalWrite(Relay_Light, HIGH);
        break;
      }
      break;

    case 14: // hour of day to do something
      switch (CurrTime.minute())
      {
      case 1:
        
        char message [] = "----- Message: Grow Lights Off -----";
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

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
