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
    RTC.init();

    Serial.begin(9600);
    // Print the format of date which will be displayed on serial monitor
    Serial.print("Date Format - DD:MM:YY");
    Serial.println(__DATE__);

    Serial.print("Time Format - HH:MM:SS");
    Serial.println(__TIME__);
    //Resets and configures RTC time registers, enables RTC interrupts
    char timeStr[3] = {0};

    memcpy(timeStr, __TIME__, 2);
    rtcTime.hours = atoi(timeStr);
    memcpy(timeStr, __TIME__+3, 2);
    rtcTime.mins  = atoi(timeStr);
    memcpy(timeStr, __TIME__+6, 2);
    rtcTime.secs  = atoi(timeStr);
    rtcTime.msecs  = 0;
    // Set the RTC time

    result = RTC.setTime(&rtcTime);
    
    if (0 == result)
    {
        delay(10);
    
        rtcDate.month   = 0;
        memcpy(timeStr, __DATE__+4, 2); //MONTH IS CHAR
        rtcDate.day = atoi(timeStr);
        memcpy(timeStr, __DATE__+9, 2);        
        rtcDate.year  = atoi(timeStr);

        // Set the RTC date
        result = RTC.setDate(&rtcDate);
        if (0 == result)
        {
            RTC.start();  //RTC starts counting the time
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
        Serial.print (":");
        Serial.print (rtcTimeRead.msecs);
        delay(1000); // Give some delay for the time to get updated
    }
}
