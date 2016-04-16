/*
 * RTC Demo
 *
 * This demo configures the RTC date, time and reads the
 * updated date and time for every 1 secs. Date and time
 * value read from RTC library shall be displayed on
 * serial monitor.
 */

#include "RTC_lib.h"

void setup()
{
    Serial.begin(9600);

    // Print the format of date which will be displayed on serial monitor
    Serial.println("Time Format - HH:MM:SS:mm");
    Serial.println("Date Format - D/M/YYYY");

    RTC.init();
    RTC.start();
    RTC.setCompileTimeAndDate(); //sets RTC to the date time the program was compiled.
}

void loop()
{
    //serially print RTC value
    Serial.print ("Current Time - ");
    Serial.print (RTC.hours());
    Serial.print (":");
    Serial.print (RTC.minutes());
    Serial.print (":");
    Serial.print (RTC.seconds());
    Serial.print (":");
    Serial.print (RTC.milliseconds());
    Serial.print ("\t");
    Serial.print (RTC.day());
    Serial.print ("/");
    Serial.print (RTC.month());
    Serial.print ("/");
    Serial.println (RTC.year());
    
    delay(1000); //wait 1s
}
