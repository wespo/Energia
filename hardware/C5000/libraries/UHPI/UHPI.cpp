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
/** @file UHPI.cpp
 *
 *  @brief UHPI implementation
 *
 */
 
#include <core.h>
#include <UHPI.h>

/** Defining the global static Member of the UHPI Class */
CSL_UhpiObj UHPI_Class::uhpiObject = {0, 0, 0};

/** Defining a global UHPI Class Object which can be used by the User
    Application */
UHPI_Class UHPI;

/** Macro to enable the Print messages to be displayed on the Serial */
//#define ENABLE_SERIAL_MSGS

#ifdef ENABLE_SERIAL_MSGS
#include <Serial_lib.h>
#endif

/** ===========================================================================
 *   @n@b UHPI_LOG_MSG_PRINT()
 *
 *   @b Description
 *   @n API to display debug messages
 *
 *   @b Arguments
 *   @verbatim
        printString - String to be displayed
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void UHPI_LOG_MSG_PRINT (char *printString);

/** ===========================================================================
 *   @n@b begin()
 *
 *   @b Description
 *   @n API to Initialize the UHPI Module
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - UHPI init successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int UHPI_Class::begin ()
{
    CSL_Status     status;
    CSL_UhpiHandle uhpiHandle;

#ifdef ENABLE_SERIAL_MSGS
    uartClass.begin(9600);
#endif

    uhpiHandle = UHPI_open(CSL_UHPI_INST_0, &(this->uhpiObject), &status);
    if ((uhpiHandle == NULL) || (status != CSL_SOK))
    {
        UHPI_LOG_MSG_PRINT("begin(): UHPI_open Failed\r\n");
    }
    else
    {
        status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE, CSL_EBSR_PPMODE_0);
        if (CSL_SOK != status)
        {
            UHPI_LOG_MSG_PRINT("begin(): SYS_setEBSR failed\r\n");
        }
    }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b end()
 *
 *   @b Description
 *   @n API to Close the instance of UHPI Module
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - UHPI Close successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int UHPI_Class::end ()
{
    CSL_Status status;

    status = UHPI_Close(&(this->uhpiObject));
    if (status != CSL_SOK)
    {
        UHPI_LOG_MSG_PRINT("end(): UHPI_Close Failed\r\n");
    }
    else
    {
        this->uhpiObject.Regs    = (CSL_UhpiRegsOvly)0x0000;
        this->uhpiObject.sysRegs = (CSL_SysRegsOvly)0x0000;
    }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b config()
 *
 *   @b Description
 *   @n API to Configure the UHPI Module
 *
 *   @b Arguments
 *   @verbatim
        gpioIntMode - GPIO interrupt mode(enable/disable) for UHPI
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - UHPI Config successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int UHPI_Class::config (Bool gpioIntMode)
{
    CSL_Status    status;
    CSL_UhpiSetup uhpiSetUp;

    if ((UHPI_GPIO_INTERRUPT_ENABLE != gpioIntMode) &&
        (UHPI_GPIO_INTERRUPT_DISABLE != gpioIntMode))
    {
        UHPI_LOG_MSG_PRINT("config(): Invalid GPIO Interrupt Mode\r\n");
        return ((int)CSL_ESYS_INVPARAMS);
    }

    uhpiSetUp.op_mode   = CSL_UHPI_16BIT_MUXED_SINGLE_FULLWORD_CYCLES;
    uhpiSetUp.gpio_intr = gpioIntMode;

    status = UHPI_Config(&(this->uhpiObject), &uhpiSetUp);
    if (status != CSL_SOK)
    {
        UHPI_LOG_MSG_PRINT("config(): UHPI_Config Failed\r\n");
    }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b UHPI_LOG_MSG_PRINT()
 *
 *   @b Description
 *   @n API to display debug messages
 *
 *   @b Arguments
 *   @verbatim
        printString - String to be displayed
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void UHPI_LOG_MSG_PRINT (char *printString)
{
#ifdef ENABLE_SERIAL_MSGS
    Serial.print(printString);
#else
    printf("%s", printString);
#endif
}
