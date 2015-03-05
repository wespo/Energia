/*
 * RTC Demo
 *
 * This demo configures the RTC date, time and reads the
 * updated date and time for every 1 secs. Date and time
 * value read from RTC library shall be displayed on
 * serial monitor.
 */

#include "RTC_lib.h"

RTCDate  rtcDateRead;
RTCTime  rtcTimeRead;
int      result;

void setup()
{
    RTCDate  rtcDate;
    RTCTime  rtcTime;
    Serial.begin(9600);
    // Print the format of date which will be displayed on serial monitor
    Serial.println("Date Format - DD:MM:YY");
    Serial.println("Time Format - HH:MM:SS");

    //Resets and configures RTC time registers, enables RTC interrupts
    RTC.init();

    rtcTime.hours = 10;
    rtcTime.mins  = 10;
    rtcTime.secs  = 0;
    rtcTime.msecs = 0;

    // Set the RTC time
    result = RTC.setTime(&rtcTime);
    if (0 == result)
    {
        delay(10);

        rtcDate.day   = 23;
        rtcDate.month = 9;
        rtcDate.year  = 13;

        // Set the RTC date
        result = RTC.setDate(&rtcDate);
        if (0 == result)
        {
            RTC.start();  //RTC starts counting the time

            delay(1000);  // Give some delay for the time to get updated
            RTC.getDate(&rtcDateRead);  // Read the date

	    Serial.print ("\nRTC Started");
            Serial.print ("\nCurrent Date - ");
            Serial.print (rtcDateRead.day);
            Serial.print (":");
            Serial.print (rtcDateRead.month);
            Serial.print (":");
            Serial.println (rtcDateRead.year);
        }
    }
}

void loop()
{
    if (0 == result)
    {
        RTC.getTime(&rtcTimeRead);  //Read RTC time

        Serial.print ("\nCurrent Time - ");
        Serial.print (rtcTimeRead.hours);
        Serial.print (":");
        Serial.print (rtcTimeRead.mins);
        Serial.print (":");
        Serial.print (rtcTimeRead.secs);

        delay(1000); // Give some delay for the time to get updated
    }
}