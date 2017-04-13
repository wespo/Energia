//////////////////////////////////////////////////////////////////////////////
// *
// * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
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
/** @file SPI.h
 *
 *  @brief SPI library header file
 *
 */

#ifndef _SPI_H_
#define _SPI_H_

#include "core.h"
#include <stdio.h>

#ifdef BOARD_DSPSHIELD_V2

#include <csl_mcspi.h>
#include <csl_general.h>
#include <csl_intc.h>
#include "csl_gpio.h"
#include "csl_mcspi.h"
#include "csl_sysctrl.h"

#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ENABLE_SERIAL_MSGS

/** Macro for SPI clock divider value 2 */
#define SPI_CLOCK_DIV2   (2)
/** Macro for SPI clock divider value 4 */
#define SPI_CLOCK_DIV4   (4)
/** Macro for SPI clock divider value 8 */
#define SPI_CLOCK_DIV8   (8)
/** Macro for SPI clock divider value 16 */
#define SPI_CLOCK_DIV16  (16)
/** Macro for SPI clock divider value 32 */
#define SPI_CLOCK_DIV32  (32)
/** Macro for SPI clock divider value 64 */
#define SPI_CLOCK_DIV64  (64)
/** Macro for SPI clock divider value 128 */
#define SPI_CLOCK_DIV128 (128)
#define    SPI_CLK_DIV         (25)  /**< SPI Clock Divisor */
#define    SPI_FRAME_LENGTH (1)   /**< SPI Frame length  */
/** Macro to indicate SPI mode 0 */
#define SPI_MODE0 (0)
/** Macro to indicate SPI mode 1 */
#define SPI_MODE1 (1)
/** Macro to indicate SPI mode 2 */
#define SPI_MODE2 (2)
/** Macro to indicate SPI mode 3 */
#define SPI_MODE3 (3)


/**< The above modes are for the following Clock Polarity(CPOL) and Clock
 *   Phase(CPHA) Selections
 *-----------------------
 *   MODE     CPOL  CPHA
 *-----------------------
 * SPI_MODE0   0     0
 * SPI_MODE1   0     1
 * SPI_MODE2   1     0
 * SPI_MODE3   1     1
 *-----------------------
 */

#define LSBFIRST (0) /**< Macro to indicate the Order of Data transfer as LSB
                          first */
#define MSBFIRST (1) /**< Macro to indicate the Order of Data transfer as MSB
                          first */

#define SPI_MASTER 1
#define SPI_SLAVE 0

#ifdef BOARD_DSPSHIELD_V2
/**
 * \brief SPI Class
 *
 *  Contains prototypes for functions in SPI DSP API library
 */
class SPI_Class_5517
{
    private:
        McSPI_Config     McSPIHwConfig;
        CSL_McSpiHandle  hMcspi;
        CSL_McSpiObj     McSpiObj;
        unsigned long tx_value;
        unsigned long rx_value;
        int order;
        mcSPIRead(unsigned long &value);
        mcSPIWrite(unsigned long value);
    public:
        void begin(int mode=SPI_MASTER);
        void end ();
        void setClockDivider (int divider);
        void setDataMode (int mode);
        void setLoopBackMode(int value);
        void setBitOrder(int order);
        int setCSPin(int order);
        
        int transfer16 (int value);
        int write (unsigned int buffer[], int length);
        int read (unsigned int buffer[], int length);
        
        unsigned long transfer (unsigned long value);
        int write (unsigned long buffer[], int length);
        int read (unsigned long buffer[], int length);
        

        int setWordLength(unsigned int length);
        int setDataDelay(unsigned int delay);
} ;

extern SPI_Class_5517 SPI;
/**< SPI class instance extern which can be used by application programs
 *   to access SPI DSP APIs
 */
#else /* BOARD_DSPSHIELD_V2 */
 /**
 * \brief SPI Class
 *
 *  Contains prototypes for functions in SPI DSP API library
 */
class SPI_Class
{
    private:
        static CSL_SpiHandle spiHandle;
        static SPI_Config    spiConfig;
        static int           order;

    public:
        void begin();
        void end ();
        void setClockDivider (int divider);
        void setDataMode (int mode);
        void setLoopBackMode(int value);
        void setBitOrder(int order);
        int setCSPin(int order);
        int transfer (int value);
        int write (unsigned int buffer[], int length);
        int read (unsigned int buffer[], int length);
        int setWordLength(unsigned int length);
        int setDataDelay(unsigned int delay);
} ;

extern SPI_Class SPI;
/**< SPI class instance extern which can be used by application programs
 *   to access SPI DSP APIs
 */
#endif /* BOARD_DSPSHIELD_V2 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _SPI_H_ */
