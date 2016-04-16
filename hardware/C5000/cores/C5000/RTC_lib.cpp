//////////////////////////////////////////////////////////////////////////////
// *
// * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
// * Copyright (C) 2011 Spectrum Digital, Incorporated
// *
// *
// *  Redistribution and use in source and binary forms, with or without
// *  modification, are permitted provided that the following conditions
// *  are met:
// *
// *    Redistributions of source code must retain the above copyright
// *    notice, this list of conditions and the following disclaimer.
// *
// *    Redistributions in binary form must reproduce the above copyright
// *    notice, this list of conditions and the following disclaimer in the
// *    documentation and/or other materials provided with the
// *    distribution.
// *
// *    Neither the name of Texas Instruments Incorporated nor the names of
// *    its contributors may be used to endorse or promote products derived
// *    from this software without specific prior written permission.
// *
// *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// *
//////////////////////////////////////////////////////////////////////////////

/** @file RTC_lib.cpp
 *
 *  @brief RTC implementation
 *
 */
 
#include "RTC_lib.h"
#include "core.h"
#include <stdio.h>
#include <string.h>


/**
 * Class identifier declaration
 */
RTCClass RTC;

/** ===========================================================================
 *   @n@b init()
 *
 *   @b Description
 *   @n Function initializes RTC Module.
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void RTCClass::init(void)
{

	CSL_RtcConfig    rtcConfig;
	started = false;
    RTC_reset();

	rtcConfig.rtcyear  = 0;
	rtcConfig.rtcmonth = 0;
	rtcConfig.rtcday   = 0;
	rtcConfig.rtchour  = 0;
	rtcConfig.rtcmin   = 0;
	rtcConfig.rtcsec   = 0;
	rtcConfig.rtcmSec  = 0;

	rtcConfig.rtcyeara  = 0;
	rtcConfig.rtcmontha = 0;
	rtcConfig.rtcdaya   = 0;
	rtcConfig.rtchoura  = 0;
	rtcConfig.rtcmina   = 0;
	rtcConfig.rtcseca   = 0;
	rtcConfig.rtcmSeca  = 0;

	RTC_config(&rtcConfig);
}

/** ===========================================================================
 *   @n@b start
 *
 *   @b Description
 *   @n Starts RTC
 *
 *  ===========================================================================
 */
void RTCClass::start(void)
{
    RTC_start();
    started = true;
}


/** ===========================================================================
 *   @n@b openRegisters
 *
 *   @b Description
 *   @n enables writing of RTC registers on C5517. Does nothing on other CPUs
 *
 *  ===========================================================================
 */
void RTCClass::openRegisters()
{
	#ifdef CHIP_C5517
		// Enable RTC Registers as writeable 
		asm(" *port(#0x1C27) = #0x0001 "); // un-isolate CVDD_RTC power domain 
		asm(" *port(#0x196C) = #0xF1E0 "); // RTC writeable unlock key 
		asm(" *port(#0x196D) = #0x95A4 ");

		delayMicroseconds(50);
	#endif
}

/** ===========================================================================
 *   @n@b closeRegisters
 *
 *   @b Description
 *   @n disables writing of RTC registers on C5517. Does nothing on other CPUs
 *
 *  ===========================================================================
 */
void RTCClass::closeRegisters()
{
	#ifdef CHIP_C5517
		delayMicroseconds(50);
		// Enable RTC Registers as writeable 
		asm(" *port(#0x1C27) = #0x0000 "); // un-isolate CVDD_RTC power domain 
		asm(" *port(#0x196C) = #0x0000 "); // RTC writeable unlock key 
		asm(" *port(#0x196D) = #0x0000 ");
	#endif
}

/** ===========================================================================
 *   @n@b stop
 *
 *   @b Description
 *   @n Stops RTC
 *
 *  ===========================================================================
 */
void RTCClass::stop(void)
{
    RTC_stop();
}

/** ===========================================================================
 *   @n@b setTime
 *
 *   @b Description
 *   @n Sets RTC time.
 *
 *   @b Arguments
 *   @verbatim
 *      pRtcTime    - Pointer to RTC time structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Set time is successful
 *   @li                    CSL_ESYS_INVPARAMS  - Time Parameters are invalid
 *
 *  ===========================================================================
 */
int RTCClass::setTime(RTCTime *pRtcTime)
{
    CSL_Status     status;
    CSL_RtcTime    rtcTime;

	status = CSL_ESYS_INVPARAMS;
	if(!started)
	{
		start();
	}
	openRegisters();

    if(pRtcTime != NULL)
	{
		rtcTime.mSecs = pRtcTime->msecs;
		rtcTime.secs  = pRtcTime->secs;
		rtcTime.mins  = pRtcTime->mins;
		rtcTime.hours = pRtcTime->hours;

        status = RTC_setTime(&rtcTime);
	}

	closeRegisters();

    return status;
}

/** ===========================================================================
 *   @n@b setTime
 *
 *   @b Description
 *   @n Sets RTC time.
 *
 *   @b Arguments
 *   @verbatim
 *      hours (0-24)
 *		minutes (0-59)
 *		seconds (0-59)
 *		milliseconds (0-999)
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Set time is successful
 *   @li                    CSL_ESYS_INVPARAMS  - Time Parameters are invalid
 *
 *  ===========================================================================
 */
int RTCClass::setTime(int hours, int minutes, int seconds, int milliseconds)
{
    CSL_Status     status;
    CSL_RtcTime    rtcTime;

	status = CSL_ESYS_INVPARAMS;
	if(!started)
	{
		start();
	}
	openRegisters();

	rtcTime.mSecs = milliseconds;
	rtcTime.secs  = seconds;
	rtcTime.mins  = minutes;
	rtcTime.hours = hours;

    status = RTC_setTime(&rtcTime);

	closeRegisters();

    return status;
}

/** ===========================================================================
 *   @n@b setDate
 *
 *   @b Description
 *   @n Sets RTC date
 *
 *   @b Arguments
 *   @verbatim
 *      pRtcDate    - Pointer to RTC date structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Set date is successful
 *   @li                    CSL_ESYS_INVPARAMS  - Date Parameters are invalid
 *
 *  ===========================================================================
 */
int RTCClass::setDate(RTCDate *pRtcDate)
{
    CSL_Status     status;
    CSL_RtcDate    rtcDate;

	status = CSL_ESYS_INVPARAMS;

	if(!started)
	{
		start();
	}

	openRegisters();
    
    if(pRtcDate != NULL)
	{
		rtcDate.day   = pRtcDate->day;
		rtcDate.month = pRtcDate->month;
		rtcDate.year  = pRtcDate->year;

        status = RTC_setDate(&rtcDate);
    }

	closeRegisters();

    return status;
}
/** ===========================================================================
 *   @n@b setDate
 *
 *   @b Description
 *   @n Reads RTC time.
 *
 *   @b Arguments
 *   @verbatim
 *      day 1-31
 *		month 1-12
 *		year 0-9999
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Reading time is successful
 *   @li                    CSL_ESYS_INVPARAMS  - Time Parameters are invalid
 *
 *  ===========================================================================
 */
int RTCClass::setDate(int day, int month, int year)
{
    CSL_Status     status;
    CSL_RtcDate    rtcDate;

	status = CSL_ESYS_INVPARAMS;

	if(!started)
	{
		start();
	}

	openRegisters();

	rtcDate.day   = day;
	rtcDate.month = month;
	rtcDate.year  = year;

    status = RTC_setDate(&rtcDate);

	closeRegisters();

    return status;
}
/** ===========================================================================
 *   @n@b getTime
 *
 *   @b Description
 *   @n Reads RTC time.
 *
 *   @b Arguments
 *   @verbatim
 *      pRtcTime    - Pointer to RTC time structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Reading time is successful
 *   @li                    CSL_ESYS_INVPARAMS  - Time Parameters are invalid
 *
 *  ===========================================================================
 */
int RTCClass::getTime(RTCTime *pRtcTime)
{
    CSL_Status     status;
    CSL_RtcTime    rtcTime;

	status = CSL_ESYS_INVPARAMS;

	if(pRtcTime != NULL)
	{
		status = RTC_getTime(&rtcTime);
		if(status == CSL_SOK)
		{
			pRtcTime->msecs = rtcTime.mSecs;
			pRtcTime->secs  = rtcTime.secs;
			pRtcTime->mins  = rtcTime.mins;
			pRtcTime->hours = rtcTime.hours;
		}
	}

    return status;
}


/** ===========================================================================
 *   @n@b hours
 *
 *   @b Description
 *   @n Reads RTC time.
 *
 *   @b Arguments
 *   @verbatim
 
     @endverbatim
 *
 *   <b> Return Value </b>  hours
 *
 *  ===========================================================================
 */
int RTCClass::hours()
{
    CSL_Status     status;
    CSL_RtcTime    rtcTime;

	status = RTC_getTime(&rtcTime);

    return rtcTime.hours;
}

/** ===========================================================================
 *   @n@b minutes
 *
 *   @b Description
 *   @n Reads RTC time.
 *
 *   @b Arguments
 *   @verbatim
 
     @endverbatim
 *
 *   <b> Return Value </b>  minutes
 *
 *  ===========================================================================
 */
int RTCClass::minutes()
{
    CSL_Status     status;
    CSL_RtcTime    rtcTime;

	status = RTC_getTime(&rtcTime);

    return rtcTime.mins;
}

/** ===========================================================================
 *   @n@b seconds
 *
 *   @b Description
 *   @n Reads RTC time.
 *
 *   @b Arguments
 *   @verbatim
 
     @endverbatim
 *
 *   <b> Return Value </b>  seconds
 *
 *  ===========================================================================
 */
int RTCClass::seconds()
{
    CSL_Status     status;
    CSL_RtcTime    rtcTime;

	status = RTC_getTime(&rtcTime);

    return rtcTime.secs;
}

/** ===========================================================================
 *   @n@b milliseconds
 *
 *   @b Description
 *   @n Reads RTC time.
 *
 *   @b Arguments
 *   @verbatim
 
     @endverbatim
 *
 *   <b> Return Value </b>  milliseconds
 *
 *  ===========================================================================
 */
int RTCClass::milliseconds()
{
    CSL_Status     status;
    CSL_RtcTime    rtcTime;

	status = RTC_getTime(&rtcTime);

    return rtcTime.mSecs;
}

/** ===========================================================================
 *   @n@b getDate
 *
 *   @b Description
 *   @n Reads RTC date
 *
 *   @b Arguments
 *   @verbatim
 *      pRtcDate    - Pointer to RTC date structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Reading date is successful
 *   @li                    CSL_ESYS_INVPARAMS  - Date Parameters are invalid
 *
 *  ===========================================================================
 */
int RTCClass::getDate(RTCDate *pRtcDate)
{
    CSL_Status     status;
    CSL_RtcDate    rtcDate;

	status = CSL_ESYS_INVPARAMS;

	if(pRtcDate != NULL)
	{
		status = RTC_getDate(&rtcDate);
		if(status == CSL_SOK)
		{
			pRtcDate->day   = rtcDate.day;
			pRtcDate->month = rtcDate.month;
			pRtcDate->year  = rtcDate.year;
		}
	}

    return status;
}

/** ===========================================================================
 *   @n@b month
 *
 *   @b Description
 *   @n Reads RTC date
 *
 *   @b Arguments
 *   @verbatim
 *      pRtcDate    - Pointer to RTC date structure
     @endverbatim
 *
 *   <b> Return Value </b>  month
 *
 *  ===========================================================================
 */
int RTCClass::month()
{
    CSL_Status     status;
    CSL_RtcDate    rtcDate;

	
	status = RTC_getDate(&rtcDate);

    return rtcDate.month;
}

/** ===========================================================================
 *   @n@b day
 *
 *   @b Description
 *   @n Reads RTC date
 *
 *   @b Arguments
 *   @verbatim
 *      pRtcDate    - Pointer to RTC date structure
     @endverbatim
 *
 *   <b> Return Value </b>  day
 *
 *  ===========================================================================
 */
int RTCClass::day()
{
    CSL_Status     status;
    CSL_RtcDate    rtcDate;

	
	status = RTC_getDate(&rtcDate);

    return rtcDate.day;
}


/** ===========================================================================
 *   @n@b year
 *
 *   @b Description
 *   @n Reads RTC date
 *
 *   @b Arguments
 *   @verbatim
 *      pRtcDate    - Pointer to RTC date structure
     @endverbatim
 *
 *   <b> Return Value </b>  year
 *
 *  ===========================================================================
 */
int RTCClass::year()
{
    CSL_Status     status;
    CSL_RtcDate    rtcDate;

	
	status = RTC_getDate(&rtcDate);

    return rtcDate.year;
}

/** ===========================================================================
 *   @n@b setAlarm
 *
 *   @b Description
 *   @n Sets RTC alarm.
 *
 *   @b Arguments
 *   @verbatim
 *      pRtcTime    - Pointer to RTC time structure
 *      pRtcDate    - Pointer to RTC date structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK
 *   @li                    CSL_ESYS_INVPARAMS
 *
 *  ===========================================================================
 */
int RTCClass::setAlarm(RTCTime *pRtcTime, RTCDate *pRtcDate)
{
    CSL_Status      status;
    CSL_RtcAlarm    rtcAlarm;

	status = CSL_ESYS_INVPARAMS;

    if((pRtcTime != NULL) && (pRtcDate != NULL))
	{
		rtcAlarm.day   = pRtcDate->day;
		rtcAlarm.month = pRtcDate->month;
		rtcAlarm.year  = pRtcDate->year;
		rtcAlarm.mSecs = pRtcTime->msecs;
		rtcAlarm.secs  = pRtcTime->secs;
		rtcAlarm.mins  = pRtcTime->mins;
		rtcAlarm.hours = pRtcTime->hours;

        status = RTC_setAlarm(&rtcAlarm);
	}

    return status;
}

/** ===========================================================================
 *   @n@b getAlarm
 *
 *   @b Description
 *   @n Reads RTC alarm time.
 *
 *   @b Arguments
 *   @verbatim
 *      pRtcTime    - Pointer to RTC time structure
 *      pRtcDate    - Pointer to RTC date structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK
 *   @li                    CSL_ESYS_INVPARAMS
 *
 *  ===========================================================================
 */
int RTCClass::getAlarm(RTCTime *pRtcTime, RTCDate *pRtcDate)
{
    CSL_Status      status;
    CSL_RtcAlarm    rtcAlarm;

	status = CSL_ESYS_INVPARAMS;

    if((pRtcTime != NULL) && (pRtcDate != NULL))
	{
		status = RTC_getAlarm(&rtcAlarm);

		pRtcDate->day   = rtcAlarm.day;
		pRtcDate->month = rtcAlarm.month;
		pRtcDate->year  = rtcAlarm.year;
		pRtcTime->msecs = rtcAlarm.mSecs;
		pRtcTime->secs  = rtcAlarm.secs;
		pRtcTime->mins  = rtcAlarm.mins;
		pRtcTime->hours = rtcAlarm.hours;
	}

    return status;
}


void RTCClass::setCompileTimeAndDate()
{
    char timeStr[3] = {0};

    memcpy(timeStr, __TIME__, 2);
    int hours = atoi(timeStr);
    memcpy(timeStr, __TIME__+3, 2);
    int mins  = atoi(timeStr);
    memcpy(timeStr, __TIME__+6, 2);
    int secs  = atoi(timeStr);

    setTime(hours, mins, secs, 0);

    char s_month[5];
    int month, day, year;
    
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    sscanf(__DATE__, "%s %d %d", s_month, &day, &year);

    month = (strstr(month_names, s_month)-month_names)/3 + 1;

    setDate(day, month, year);
}


