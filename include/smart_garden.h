#ifndef SMART_GARDEN_H
#define SMART_GARDEN_H

#include <RTClib.h>
#include <SPI.h>
#include <SD.h>

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
void log_msg(char const *message, DateTime const &CurrTime);
void turn_lights_on(DateTime const &CurrTime);
void turn_lights_off(DateTime const &CurrTime);

#endif // SMART_GARDEN_H