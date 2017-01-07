/*! @file RTC.cpp
 * 
 * @brief Configures RTC date/time & reads value
 *
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

/// Configure RTC date/time
void setup()
{
    RTCDate  rtcDate;
    RTCTime  rtcTime;
    Serial.begin(9600);
    // Print the format of date which will be displayed on serial monitor
    Serial.println("Date Format - DD:MM:YY");
    Serial.println("Time Format - HH:MM:SS");

    RTC.init();

    rtcTime.hours = 10;
    rtcTime.mins  = 10;
    rtcTime.secs  = 0;
    rtcTime.msecs = 0;

    result = RTC.setTime(&rtcTime);
    if (0 == result)
    {
        delay(10);

        rtcDate.day   = 23;
        rtcDate.month = 9;
        rtcDate.year  = 13;

        result = RTC.setDate(&rtcDate);
        if (0 == result)
        {


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
    {

        Serial.print ("\nCurrent Time - ");
        Serial.print (rtcTimeRead.hours);
        Serial.print (":");
        Serial.print (rtcTimeRead.mins);
        Serial.print (":");
        Serial.print (rtcTimeRead.secs);

    }
}