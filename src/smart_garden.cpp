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
#include "smart_garden.h"

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
  Serial.begin(9600);
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    error(10);
  }

  if (!rtc.initialized()) // If RTC is no running
  {
    Serial.println("RTC IS NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // try to adjust time anyways
  }
  else // if RTC is running
  {
    Serial.println("RTC IS running!");
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

  // No more room, all files exist. Purge
  char match[1];
  strcpy(match, "9");
  if ((filename[7] == match[0]) && (filename[8] == match[0]))
  {
    for (uint8_t i = 0; i < 99; i++)
    {
      filename[7] = '0' + i / 10;
      filename[8] = '0' + i % 10;
      SD.remove(filename);
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
  char message[] = "----- Message: System Power On -----";
  log_msg(message, PowerTime);

  Serial.print("PowerTime: ");
  Serial.print(PowerTime.year(), DEC);
  Serial.print('/');
  Serial.print(PowerTime.month(), DEC);
  Serial.print('/');
  Serial.print(PowerTime.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[PowerTime.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(PowerTime.hour(), DEC);
  Serial.print(':');
  Serial.print(PowerTime.minute(), DEC);
  Serial.print(':');
  Serial.print(PowerTime.second(), DEC);
  Serial.println();
  /***** END: Initial Message. Power On. *****/

  /***** If we get here success *****/
  digitalWrite(LED_1, HIGH);
}

// define the infinite loop function
static bool BootUpLightsOn = false;
static bool BootUpLightsOff = false;
static bool ScheduledLightsOn = false;
static bool ScheduledLightsOff = false;
void loop()
{

  DateTime CurrTime = rtc.now(); // grabs the current time from the RTC

  Serial.print("CurrTime: ");
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

  DateTime LightsOnTime = DateTime(0, 0, 0, 3, 0, 1);   // 3:00:01 am
  DateTime LightsOffTime = DateTime(0, 0, 0, 17, 0, 1); // 17:00:01 pm

  auto turn_lighs_on = (CurrTime.hour() == LightsOnTime.hour()) &&
                       (CurrTime.minute() == LightsOnTime.minute()) &&
                       (CurrTime.second() >= LightsOnTime.second());

  auto turn_lighs_off = (CurrTime.hour() == LightsOffTime.hour()) &&
                        (CurrTime.minute() == LightsOffTime.minute()) &&
                        (CurrTime.second() >= LightsOffTime.second());

  auto passed_turn_lighs_on =
      // Check are we passed Turn On Time
      ((CurrTime.hour() >= LightsOnTime.hour()) &&
       (CurrTime.minute() >= LightsOnTime.minute()) &&
       (CurrTime.second() >= LightsOnTime.second())) &&

      // Check are we before Turn Off Time
      ((CurrTime.hour() <= LightsOffTime.hour()) &&
       (CurrTime.minute() <= LightsOffTime.minute()) &&
       (CurrTime.second() <= LightsOffTime.second()));

  auto passed_turn_lighs_off =
      // Check are we passed Turn Off Time
      ((CurrTime.hour() >= LightsOffTime.hour()) &&
       (CurrTime.minute() >= LightsOffTime.minute()) &&
       (CurrTime.second() >= LightsOffTime.second())) ||

      // Check are we before turn On Time
      ((CurrTime.hour() <= LightsOnTime.hour()) &&
       (CurrTime.minute() <= LightsOnTime.minute()) &&
       (CurrTime.second() <= LightsOnTime.second()));

  // Let's assume we just powered on the arduino. If we have passed our on time lets turn it on
  if (passed_turn_lighs_on && !BootUpLightsOn)
  {
    turn_lights_on(CurrTime);
    BootUpLightsOn = true;
    Serial.println("Grow Lights on per first boot...");
  }

  // Let's assume we just powered on the arduino. If we have passed our off time lets turn it off
  if (passed_turn_lighs_off && !BootUpLightsOff)
  {
    turn_lights_off(CurrTime);
    BootUpLightsOff = true;
    Serial.println("Grow Lights off per first boot...");
  }

  // If our time on has come
  if (turn_lighs_on && !ScheduledLightsOn)
  {
    turn_lights_on(CurrTime);
    ScheduledLightsOn = true;
    ScheduledLightsOff = false;
    Serial.println("Grow Lights on per schedule...");
  }

  // If our time off has come
  if (turn_lighs_off && !ScheduledLightsOff)
  {
    turn_lights_off(CurrTime);
    ScheduledLightsOn = false;
    ScheduledLightsOff = true;
    Serial.println("Grow Lights off per schedule...");
  }

  /***** BEGIN: Sleep when there is nothing else to do. *****/
  digitalWrite(LED_1, LOW);
  delay(1000); // waits for a second
  digitalWrite(LED_1, HIGH);
  /***** BEGIN: Sleep when there is nothing else to do. *****/
}

/* Function for logging to SD card */
void log_msg(char const *message, DateTime const &CurrTime)
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

void printDirectory(File dir, int numTabs)
{
  while (true)
  {

    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++)
    {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void turn_lights_on(DateTime const &CurrTime)
{
  char message[] = "----- Message: Grow Lights On -----";
  log_msg(message, CurrTime);
  digitalWrite(Relay_Light, HIGH);
}

void turn_lights_off(DateTime const &CurrTime)
{
  char message[] = "----- Message: Grow Lights Off -----";
  log_msg(message, CurrTime);
  digitalWrite(Relay_Light, LOW);
}
