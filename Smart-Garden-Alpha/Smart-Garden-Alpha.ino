/* First program for smart-garden.
 *  Goals: (1) Grab time from RTC, water every 3 days for 10 seconds
 *         (2) Water on command with Button, Reset counter for days          
 */

/* Libraries for RTC */
#include <Wire.h>
#include "RTClib.h"
/* End of Linraries for RTC */

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; /* array for weekdays and time */

void setup()
{
  /* Code verifies serial com is set up */
   Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) 
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  /* End of serial com set up */
}

void loop() {
  // put your main code here, to run repeatedly:

}
