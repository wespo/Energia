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
/** @file SPI.cpp
 *
 *  @brief SPI implementation
 *
 */

#include <SPI.h>


#ifdef BOARD_DSPSHIELD_V2

/** Defining a global SPI Class Object which can be used by the User
    Application */
SPI_Class_5517 SPI;

//#define EBSR 0x1C00
/** Macro to enable the Print messages to be displayed on the Serial */
//#define ENABLE_SERIAL_MSGS

#ifdef ENABLE_SERIAL_MSGS
#include <Serial_lib.h>
#endif

/** ===========================================================================
 *   @n@b SPI_LOG_MSG_PRINT()
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
void SPI_LOG_MSG_PRINT (char *printString);
int mcSPIWrite(unsigned long value);
int mcSPIRead(unsigned long &value);


/** ===========================================================================
 *   @n@b begin()
 *
 *   @b Description
 *   @n API to Initialize the SPI Module
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
void SPI_Class_5517::begin (int mode)
{
    
    pinMode(ARD_SPI_EN, OUTPUT);
    digitalWrite(ARD_SPI_EN, HIGH);

    CSL_Status Status;
    // volatile Uint32 i=0;
    //ioExpander_Setup();

    //Call this fn for enabling

    // port,pin, value
    //CSL_SYSCTRL_REGS->CLKSTOP2 = 0;
    hMcspi =  McSPI_open(McSPI_POLLING_MODE,&McSpiObj,&Status);
    Status = SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,CSL_EBSR_SP1MODE_1);
    // *(ioport volatile unsigned *)EBSR&= 0xF3FF;
    // //*(ioport volatile unsigned *)EBSR|= 0x0400;
    // *(ioport volatile unsigned *)EBSR|= 0x0800;
    if(CSL_SOK != Status)
    {
        SPI_LOG_MSG_PRINT("SYS_setEBSR failed\n");
    }
    McSPI_close(hMcspi);  // close if clock already started
  hMcspi =  McSPI_open(McSPI_POLLING_MODE,&McSpiObj,&Status);

  if ( (hMcspi == NULL) || (Status != CSL_SOK) )
  {
   SPI_LOG_MSG_PRINT("mcspi_open() Failed \n");
  }
  else
  {
    SPI_LOG_MSG_PRINT("mcspi_open() Success \n");
  }

    Status = SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,CSL_EBSR_SP1MODE_1);
    if(CSL_SOK != Status)
    {
        SPI_LOG_MSG_PRINT("SYS_setEBSR failed\n");
    }

    hMcspi->McSPIHwConfig.mode       = McSPI_MASTER;//(McSPI_mode)mode;
    hMcspi->McSPIHwConfig.duplex     = McSPI_FULL_DUPLEX;
    hMcspi->McSPIHwConfig.wLen       = McSPI_WORD_LENGTH_8;
    hMcspi->McSPIHwConfig.dataDelay  = McSPI_DATA_DLY4;
    hMcspi->McSPIHwConfig.ClkDiv     = CSL_MCSPI_CH0CONFL_CLKD_DIV64;//4;
    hMcspi->McSPIHwConfig.clkPh      = McSPI_CLK_PH_EVEN_EDGE;   //even edge ,polarity low important
    hMcspi->McSPIHwConfig.clkPol     = McSPI_CLKP_LOW;
    hMcspi->ch                       = McSPI_CS_NUM_0;
    hMcspi->McSPIHwConfig.csPol      = McSPI_CSP_ACTIVE_LOW;
    hMcspi->McSPIHwConfig.inSel      = McSPI_RX_SPIDAT0;
    hMcspi->McSPIHwConfig.enDat0_1   = (McSPI_TxEn_Dat0_1)McSPI_TX_ENABLE_SPIDAT0;


    Status = McSPI_config(hMcspi);
    if( CSL_SOK != Status )
    {
     SPI_LOG_MSG_PRINT("\r\n McSPI_Config() is failed\r\n");
    }

    // tx_value = 0x0; // u45 pin 9
    // Uint32 flagBuf = CSL_MCSPI_REGS->CH0STATL;
    // CSL_MCSPI_REGS->CH0STATL = flagBuf & CSL_MCSPI_CH0STATL_RXS_MASK;
    order = 0;
}

/** ===========================================================================
 *   @n@b end()
 *
 *   @b Description
 *   @n API to disable the SPI Module
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
void SPI_Class_5517::end ()
{
    CSL_Status status;

    status = McSPI_close(hMcspi);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("end(): SPI Instance De-Intialization Failed\r\n");
        return;
    }
    hMcspi = (CSL_McSpiHandle)NULL;
}

/** ===========================================================================
 *   @n@b setClockDivider()
 *
 *   @b Description
 *   @n API to set the SPI clock divider relative to the system clock
 *
 *   @b Arguments
 *   @verbatim
        divider - The clock divider value
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void SPI_Class_5517::setClockDivider (int divider)
{
    CSL_Status status;

    hMcspi->McSPIHwConfig.ClkDiv = (Uint16)divider;

    status = McSPI_config(hMcspi);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setClockDivider(): setClockDivider() API Failed");
        SPI_LOG_MSG_PRINT ("\r\n");
    }
}

/** ===========================================================================
 *   @n@b setDataMode()
 *
 *   @b Description
 *   @n API to set the clock, polarity and phase
 *
 *   @b Arguments
 *   @verbatim
        mode - Mode to be set
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void SPI_Class_5517::setDataMode (int mode)
{
    CSL_Status status;

    /* The modes are for the following Clock Polarity(CPOL) and Clock
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

    switch (mode)
    {
        case SPI_MODE0:
            /* Set Clock Polarity as 0 and Clock Phase as 0 */
            hMcspi->McSPIHwConfig.clkPol = McSPI_CLKP_HIGH;
            hMcspi->McSPIHwConfig.clkPh  = McSPI_CLK_PH_ODD_EDGE;
            break;

        case SPI_MODE1:
            /* Set Clock Polarity as 0 and Clock Phase as 1 */
            hMcspi->McSPIHwConfig.clkPol = McSPI_CLKP_HIGH;
            hMcspi->McSPIHwConfig.clkPh  = McSPI_CLK_PH_EVEN_EDGE;
            break;

        case SPI_MODE2:
            /* Set Clock Polarity as 1 and Clock Phase as 0 */
            hMcspi->McSPIHwConfig.clkPol = McSPI_CLKP_LOW;
            hMcspi->McSPIHwConfig.clkPh  = McSPI_CLK_PH_ODD_EDGE;
            break;

        case SPI_MODE3:
            /* Set Clock Polarity as 1 and Clock Phase as 1 */
            hMcspi->McSPIHwConfig.clkPol = McSPI_CLKP_LOW;
            hMcspi->McSPIHwConfig.clkPh  = McSPI_CLK_PH_EVEN_EDGE;
            break;

        default:
            SPI_LOG_MSG_PRINT("setDataMode(): Invalid mode for setDataMode()");
            SPI_LOG_MSG_PRINT(" API\r\n");
            return;
    }

    status = McSPI_config(hMcspi);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setDataMode(): SPI_config() API Failed\r\n");
    }
}

/** ===========================================================================
 *   @n@b setLoopBackMode()
 *
 *   @b Description
 *   @n API to set the Loop Back Mode of the SPI
 *
 *   @b Arguments
 *   @verbatim
        value - Value (0 or 1) that is to be set for the Loop Back field
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void SPI_Class_5517::setLoopBackMode(int value)
{
    //no loopback in McSPI. Possible to implement in software later.
}


/** ===========================================================================
 *   @n@b setCSPin()
 *
 *   @b Description
 *   @n API to set the Loop Back Mode of the SPI
 *
 *   @b Arguments
 *   @verbatim
        value - Value (0 or 1) that is to be set for the Loop Back field
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
int SPI_Class_5517::setCSPin(int value)
{
    CSL_Status status;

    if(value >= McSPI_CS_NUM_INVALID)
    {
        return CSL_FAIL;
    }

    McSpiObj.ch        = (McSPI_ch) value;

    status = McSPI_config(hMcspi);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setClockDivider(): setClockDivider() API Failed");
        SPI_LOG_MSG_PRINT ("\r\n");
    }
    return ((int) status);
}

/** ===========================================================================
 *   @n@b setBitOrder()
 *
 *   @b Description
 *   @n API to set the order of the bits shifted out of and into the SPI bus,
 *      either Least-significant bit(LSB) first or Most-significant bit(MSB)
 *      first.
 *
 *   @b Arguments
 *   @verbatim
        newOrder - Value (LSBFIRST or MSBFIRST) that is to be set for the order
                   of data transfer
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void SPI_Class_5517::setBitOrder(int newOrder)
{
    if ((LSBFIRST == newOrder) || (MSBFIRST == newOrder))
    {
        order = newOrder;
    }
}

/** ===========================================================================
 *   @n@b swapWords()
 *
 *   @b Description
 *   @n Function to swap the words in a 32 bit integer value
 *
 *   @b Arguments
 *   @verbatim
        buffer - Array of 32 bit integers, which has to be word swapped
        length - Length of the input Buffer
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
static void swapWords(unsigned long *buffer, int length)
{
    int           index;
    unsigned long temp;

    index = 0;
    while (index < length)
    {
        temp = (buffer[index] & 0xFFFF);
        temp = temp << 16;
        buffer[index] = (buffer[index] >> 16);
        buffer[index] = buffer[index] | temp;

        index++;
    }
}

/** ===========================================================================
 *   @n@b swap8Bits()
 *
 *   @b Description
 *   @n Function to swap the bits in a 8 bit integer value, Most Significant
 *      Bits are swapped with Least Significant Bits
 *
 *   @b Arguments
 *   @verbatim
        buffer - Array of 8 bit integers, which has to be bit swapped
        length - Length of the input Buffer
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
static void swap8Bits(unsigned short *buffer, int length)
{
    int            index;
    int            bitIndex;
    unsigned short temp;

    index = 0;

    while (index < length)
    {
        temp = buffer[index];
        buffer[index] = 0;
        for (bitIndex = 0; bitIndex < 8; bitIndex++)
        {
            buffer[index] = buffer[index] << 1;
            buffer[index] |= (temp & 1);
            temp = temp >> 1;
        }

        index++;
    }
}

/** ===========================================================================
 *   @n@b swap32Bits()
 *
 *   @b Description
 *   @n Function to swap the bits in a 32 bit integer value, Most Significant
 *      Bits are swapped with Least Significant Bits
 *
 *   @b Arguments
 *   @verbatim
        buffer - Array of 32 bit integers, which has to be bit swapped
        length - Length of the input Buffer
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
static void swap32Bits(unsigned long *buffer, int length)
{
    int           index;
    int           bitIndex;
    unsigned long temp;

    index = 0;

    while (index < length)
    {
        temp = buffer[index];
        buffer[index] = 0;
        for (bitIndex = 0; bitIndex < 32; bitIndex++)
        {
            buffer[index] = buffer[index] << 1;
            buffer[index] |= (temp & 1);
            temp = temp >> 1;
        }

        index++;
    }
}

/** ===========================================================================
 *   @n@b transfer()
 *
 *   @b Description
 *   @n API to transfer one byte over the SPI bus, both sending and receiving
 *
 *   @b Arguments
 *   @verbatim
        value - the byte to send out over the bus
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @li             On Success - The byte read from the bus
      @li             On Failure - CSL_ESYS_INVPARAMS
 *
 *  ===========================================================================
 */
unsigned long SPI_Class_5517::transfer (unsigned long value)
{
    // mcSPIWrite(value);
    // mcSPIRead(value);
    if(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_TXS_MASK) // Check if TXBUFF is EMPTY
    {
        CSL_MCSPI_REGS->CH0TXL = (Uint16)value;
        CSL_MCSPI_REGS->CH0TXU = (Uint16)(value>>16) ;
        delayMicroseconds(10); //time for transmission

        while(!(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_EOT_MASK)); // WAIT till END of Txn

        if(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_RXS_MASK)          // Check if Data is Received
        {
            value = hMcspi->Regs->CH0RXL;

            unsigned long flags = CSL_MCSPI_REGS->CH0STATL;
            CSL_MCSPI_REGS->CH0STATL = flags & CSL_MCSPI_CH0STATL_RXS_MASK; // Clear RXFULL Mask
            //CSL_MCSPI_REGS->CH0STATL = CSL_MCSPI_CH0STATL_RXS_MASK; // Clear RXFULL Mask
            return value;
        }
    }
    return -1;
}

/** ===========================================================================
 *   @n@b transfer16()
 *
 *   @b Description
 *   @n API to transfer one byte over the SPI bus, both sending and receiving
 *
 *   @b Arguments
 *   @verbatim
        value - the byte to send out over the bus
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @li             On Success - The byte read from the bus
      @li             On Failure - CSL_ESYS_INVPARAMS
 *
 *  ===========================================================================
 */
int SPI_Class_5517::transfer16 (int value)
{
    unsigned long bufVal = (unsigned long) value;
    transfer(value);
    return value;
}
/** ===========================================================================
 *   @n@b write()
 *
 *   @b Description
 *   @n API to write an array of data to the SPI
 *
 *   @b Arguments
 *   @verbatim
        buffer [IN] Buffer that holds the data to be written
        length [IN] Length of the Buffer to be written
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI write successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int SPI_Class_5517::write (unsigned long *buffer, int length)
{
    for(int i = 0; i < length; i++)
    {
        transfer(buffer[i]);
    }
    return 1;
}

/** ===========================================================================
 *   @n@b write()
 *
 *   @b Description
 *   @n API to write an array of data to the SPI
 *
 *   @b Arguments
 *   @verbatim
        buffer [IN] Buffer that holds the data to be written
        length [IN] Length of the Buffer to be written
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI write successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int SPI_Class_5517::write (unsigned int *buffer, int length)
{
    for(int i = 0; i < length; i++)
    {
        transfer16(buffer[i]);
    }
    return 1;
}

/** ===========================================================================
 *   @n@b read()
 *
 *   @b Description
 *   @n API to read an array of data from the SPI
 *
 *   @b Arguments
 *   @verbatim
        buffer [IN] Buffer that will holds the read data
        length [IN] Length of the Buffer to be read
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI read successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int SPI_Class_5517::read (unsigned long *buffer, int length)
{   
    for(int i = 0; i < length; i++)
    {
        buffer[i] = transfer(0x00000000);
    }
}

/** ===========================================================================
 *   @n@b read()
 *
 *   @b Description
 *   @n API to read an array of data from the SPI
 *
 *   @b Arguments
 *   @verbatim
        buffer [IN] Buffer that will holds the read data
        length [IN] Length of the Buffer to be read
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI read successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int SPI_Class_5517::read (unsigned int *buffer, int length)
{   
    for(int i = 0; i < length; i++)
    {
        buffer[i] = transfer16(0x0000);
    }
}

/** ===========================================================================
 *   @n@b wordLength()
 *
 *   @b Description
 *   @n API to set the wordLength of SPI bus
 *
 *   @b Arguments
 *   @verbatim
        length [IN] spi word length in bits
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI read successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */

int SPI_Class_5517::setWordLength(unsigned int length)
{
    CSL_Status status;

    hMcspi->McSPIHwConfig.wLen = (McSPI_WordLen)length;

    status = McSPI_config(hMcspi);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setWordLength(): setWordLength() API Failed");
        SPI_LOG_MSG_PRINT ("\r\n");
    }
}


/** ===========================================================================
 *   @n@b setDataDelay()
 *
 *   @b Description
 *   @n API to set the wordLength of SPI bus
 *
 *   @b Arguments
 *   @verbatim
        delay [IN] number of bits to delay 0-3
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI read successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */

int SPI_Class_5517::setDataDelay(unsigned int delay)
{
    CSL_Status status;

    hMcspi->McSPIHwConfig.dataDelay = (McSPI_DataDly)delay;

    status = McSPI_config(hMcspi);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setDataDelay(): setDataDelay() API Failed");
        SPI_LOG_MSG_PRINT ("\r\n");
    }
}

/** ===========================================================================
 *   @n@b SPI_LOG_MSG_PRINT()
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
void SPI_LOG_MSG_PRINT (char *printString)
{
#ifdef ENABLE_SERIAL_MSGS
    Serial.print(printString);
#else
    printf("%s", printString);
#endif
}

int SPI_Class_5517::mcSPIWrite(unsigned long value)
{
  if(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_TXS_MASK) // Check if TXBUFF is EMPTY
  {
    CSL_MCSPI_REGS->CH0TXL = (Uint16)value;
    CSL_MCSPI_REGS->CH0TXU = (Uint16)(value>>16) ;
    delayMicroseconds(500); //time for transmission
    // Serial.print("\r\nMcSPI Tx data : ");
    // Serial.println(value);//need this line for delay
    while(!(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_EOT_MASK)); // WAIT till END of Txn
    return 1;
  }
  return 0;
}

int SPI_Class_5517::mcSPIRead(unsigned long &value)
{
  if(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_TXS_MASK) // Check if TXBUFF is EMPTY
  {
    //Serial.println("TX Empty");
    if(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_RXS_MASK)          // Check if Data is Received
    {
      //Serial.println("RX Avail");
      value = hMcspi->Regs->CH0RXL;
      // Serial.print("McSPI Rx data : ");
      // Serial.println(value);
      unsigned long flags = CSL_MCSPI_REGS->CH0STATL;
      CSL_MCSPI_REGS->CH0STATL = flags & CSL_MCSPI_CH0STATL_RXS_MASK; // Clear RXFULL Mask
      return 1;
    }
    return 0;
  }
  return 0;
}
#else /* BOARD_DSPSHIELD_V2 */

/** Defining the SPI Class global Members */
CSL_SpiHandle SPI_Class::spiHandle;
SPI_Config    SPI_Class::spiConfig;
int           SPI_Class::order = 1;

/** Defining a global SPI Class Object which can be used by the User
    Application */
SPI_Class SPI;

/** Macro to enable the Print messages to be displayed on the Serial */
//#define ENABLE_SERIAL_MSGS

#ifdef ENABLE_SERIAL_MSGS
#include <Serial_lib.h>
#endif

/** ===========================================================================
 *   @n@b SPI_LOG_MSG_PRINT()
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
void SPI_LOG_MSG_PRINT (char *printString);

/** ===========================================================================
 *   @n@b begin()
 *
 *   @b Description
 *   @n API to Initialize the SPI Module
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
void SPI_Class::begin ()
{
    CSL_Status status;

    status = SPI_init();
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT("begin(): SPI Initialization Failed\r\n");
    }
    else
    {
        status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE, CSL_EBSR_PPMODE_1);
        if(CSL_SOK != status)
        {
            SPI_LOG_MSG_PRINT("begin(): SYS_setEBSR failed\r\n");
        }
        else
        {
            spiHandle = SPI_open(SPI_CS_NUM_0, SPI_POLLING_MODE);
            if(NULL == spiHandle)
            {
                SPI_LOG_MSG_PRINT ("begin(): SPI Instance Open Failed\r\n");
            }
            else
            {
                /* Set the SPI hardware configuration */
                spiConfig.spiClkDiv    = SPI_CLOCK_DIV128;
                spiConfig.wLen        = SPI_WORD_LENGTH_8;
                spiConfig.frLen        = SPI_FRAME_LENGTH;
                spiConfig.wcEnable    = SPI_WORD_IRQ_ENABLE;
                spiConfig.fcEnable    = SPI_FRAME_IRQ_DISABLE;
                spiConfig.csNum        = SPI_CS_NUM_0;
                spiConfig.dataDelay    = SPI_DATA_DLY_0;
                spiConfig.csPol        = SPI_CSP_ACTIVE_LOW;
                spiConfig.clkPol    = SPI_CLKP_LOW_AT_IDLE;
                spiConfig.clkPh        = SPI_CLK_PH_FALL_EDGE;

                status = SPI_config(spiHandle, &spiConfig);
                if(CSL_SOK != status)
                {
                    SPI_LOG_MSG_PRINT ("begin(): SPI Instance Configured ");
                    SPI_LOG_MSG_PRINT ("Failed\r\n");
                }
            }
        }
    }
}

/** ===========================================================================
 *   @n@b end()
 *
 *   @b Description
 *   @n API to disable the SPI Module
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
void SPI_Class::end ()
{
    CSL_Status status;

    status = SPI_deInit();
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("end(): SPI Instance De-Intialization Failed\r\n");
    }
    else
    {
        status = SPI_close(spiHandle);
        if(CSL_SOK != status)
        {
            SPI_LOG_MSG_PRINT ("end(): SPI Instance Close Failed\r\n");
        }

        spiHandle = (CSL_SpiHandle)NULL;
    }
}

/** ===========================================================================
 *   @n@b setClockDivider()
 *
 *   @b Description
 *   @n API to set the SPI clock divider relative to the system clock
 *
 *   @b Arguments
 *   @verbatim
        divider - The clock divider value
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void SPI_Class::setClockDivider (int divider)
{
    CSL_Status status;

    spiConfig.spiClkDiv = (Uint16)divider;

    status = SPI_config(spiHandle, &spiConfig);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setClockDivider(): setClockDivider() API Failed");
        SPI_LOG_MSG_PRINT ("\r\n");
    }
}

/** ===========================================================================
 *   @n@b setDataMode()
 *
 *   @b Description
 *   @n API to set the clock, polarity and phase
 *
 *   @b Arguments
 *   @verbatim
        mode - Mode to be set
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void SPI_Class::setDataMode (int mode)
{
    CSL_Status status;

    /* The modes are for the following Clock Polarity(CPOL) and Clock
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

    switch (mode)
    {
        case SPI_MODE0:
            /* Set Clock Polarity as 0 and Clock Phase as 0 */
            spiConfig.clkPol = SPI_CLKP_LOW_AT_IDLE;
            spiConfig.clkPh  = SPI_CLK_PH_FALL_EDGE;
            break;

        case SPI_MODE1:
            /* Set Clock Polarity as 0 and Clock Phase as 1 */
            spiConfig.clkPol = SPI_CLKP_LOW_AT_IDLE;
            spiConfig.clkPh  = SPI_CLK_PH_RISE_EDGE;
            break;

        case SPI_MODE2:
            /* Set Clock Polarity as 1 and Clock Phase as 0 */
            spiConfig.clkPol = SPI_CLKP_HIGH_AT_IDLE;
            spiConfig.clkPh  = SPI_CLK_PH_FALL_EDGE;
            break;

        case SPI_MODE3:
            /* Set Clock Polarity as 1 and Clock Phase as 1 */
            spiConfig.clkPol = SPI_CLKP_HIGH_AT_IDLE;
            spiConfig.clkPh  = SPI_CLK_PH_RISE_EDGE;
            break;

        default:
            SPI_LOG_MSG_PRINT("setDataMode(): Invalid mode for setDataMode()");
            SPI_LOG_MSG_PRINT(" API\r\n");
            return;
    }

    status = SPI_config(spiHandle, &spiConfig);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setDataMode(): SPI_config() API Failed\r\n");
    }
}

/** ===========================================================================
 *   @n@b setLoopBackMode()
 *
 *   @b Description
 *   @n API to set the Loop Back Mode of the SPI
 *
 *   @b Arguments
 *   @verbatim
        value - Value (0 or 1) that is to be set for the Loop Back field
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void SPI_Class::setLoopBackMode(int value)
{
    SPI_setLoopBackMode(value);
}


/** ===========================================================================
 *   @n@b setCSPin()
 *
 *   @b Description
 *   @n API to set the Loop Back Mode of the SPI
 *
 *   @b Arguments
 *   @verbatim
        value - Value (0 or 1) that is to be set for the Loop Back field
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
int SPI_Class::setCSPin(int value)
{
    CSL_Status status;

    if(value >= SPI_CS_NUM_INVALID)
    {
        return CSL_FAIL;
    }

    spiConfig.csNum        = (SPI_HwMode) value;

    status = SPI_config(spiHandle, &spiConfig);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setClockDivider(): setClockDivider() API Failed");
        SPI_LOG_MSG_PRINT ("\r\n");
    }
    return ((int) status);
}

/** ===========================================================================
 *   @n@b setBitOrder()
 *
 *   @b Description
 *   @n API to set the order of the bits shifted out of and into the SPI bus,
 *      either Least-significant bit(LSB) first or Most-significant bit(MSB)
 *      first.
 *
 *   @b Arguments
 *   @verbatim
        newOrder - Value (LSBFIRST or MSBFIRST) that is to be set for the order
                   of data transfer
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void SPI_Class::setBitOrder(int newOrder)
{
    if ((LSBFIRST == newOrder) || (MSBFIRST == newOrder))
    {
        order = newOrder;
    }
}

/** ===========================================================================
 *   @n@b swapWords()
 *
 *   @b Description
 *   @n Function to swap the words in a 32 bit integer value
 *
 *   @b Arguments
 *   @verbatim
        buffer - Array of 32 bit integers, which has to be word swapped
        length - Length of the input Buffer
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
static void swapWords(unsigned long *buffer, int length)
{
    int           index;
    unsigned long temp;

    index = 0;
    while (index < length)
    {
        temp = (buffer[index] & 0xFFFF);
        temp = temp << 16;
        buffer[index] = (buffer[index] >> 16);
        buffer[index] = buffer[index] | temp;

        index++;
    }
}

/** ===========================================================================
 *   @n@b swap8Bits()
 *
 *   @b Description
 *   @n Function to swap the bits in a 8 bit integer value, Most Significant
 *      Bits are swapped with Least Significant Bits
 *
 *   @b Arguments
 *   @verbatim
        buffer - Array of 8 bit integers, which has to be bit swapped
        length - Length of the input Buffer
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
static void swap8Bits(unsigned short *buffer, int length)
{
    int            index;
    int            bitIndex;
    unsigned short temp;

    index = 0;

    while (index < length)
    {
        temp = buffer[index];
        buffer[index] = 0;
        for (bitIndex = 0; bitIndex < 8; bitIndex++)
        {
            buffer[index] = buffer[index] << 1;
            buffer[index] |= (temp & 1);
            temp = temp >> 1;
        }

        index++;
    }
}

/** ===========================================================================
 *   @n@b swap32Bits()
 *
 *   @b Description
 *   @n Function to swap the bits in a 32 bit integer value, Most Significant
 *      Bits are swapped with Least Significant Bits
 *
 *   @b Arguments
 *   @verbatim
        buffer - Array of 32 bit integers, which has to be bit swapped
        length - Length of the input Buffer
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
static void swap32Bits(unsigned long *buffer, int length)
{
    int           index;
    int           bitIndex;
    unsigned long temp;

    index = 0;

    while (index < length)
    {
        temp = buffer[index];
        buffer[index] = 0;
        for (bitIndex = 0; bitIndex < 32; bitIndex++)
        {
            buffer[index] = buffer[index] << 1;
            buffer[index] |= (temp & 1);
            temp = temp >> 1;
        }

        index++;
    }
}

/** ===========================================================================
 *   @n@b transfer()
 *
 *   @b Description
 *   @n API to transfer one byte over the SPI bus, both sending and receiving
 *
 *   @b Arguments
 *   @verbatim
        value - the byte to send out over the bus
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @li             On Success - The byte read from the bus
      @li             On Failure - CSL_ESYS_INVPARAMS
 *
 *  ===========================================================================
 */
int SPI_Class::transfer (int value)
{
    CSL_Status    status;
    unsigned long buffer;

    if (LSBFIRST == order)
    {
        swap8Bits ((unsigned short *)&value, 1);
    }
    buffer = value;

    /* Send the User data */
    status = SPI_dataTransaction(spiHandle, (Uint16 *)&buffer, 1, SPI_WRITE);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("transfer(): SPI Write Failed\r\n");
        return ((int)status);
    }

    /* Receive the data, which is to be sent back to the User */
    status = SPI_dataTransaction(spiHandle, (Uint16 *)&buffer, 1, SPI_READ);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("transfer(): SPI Read Failed\r\n");
        return ((int)status);
    }

    swapWords (&buffer, 1);
    value = buffer;

    if (LSBFIRST == order)
    {
        swap8Bits ((unsigned short *)&value, 1);
    }

    return ((int)value);
}

/** ===========================================================================
 *   @n@b write()
 *
 *   @b Description
 *   @n API to write an array of data to the SPI
 *
 *   @b Arguments
 *   @verbatim
        buffer [IN] Buffer that holds the data to be written
        length [IN] Length of the Buffer to be written
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI write successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int SPI_Class::write (unsigned int *buffer, int length)
{
    CSL_Status status;

    if ((NULL == buffer) || (length <= 0))
    {
        SPI_LOG_MSG_PRINT("write(): Invalid Parameter\r\n");
        return ((int)CSL_ESYS_INVPARAMS);
    }

   //  if (LSBFIRST == order)
   //  {
         // If the User requests to send LSB first, then swap all the 32 bits
         //   of the User data 
   //      swap32Bits (buffer, length);
   //  }

    status = SPI_dataTransaction(spiHandle, (Uint16 *)buffer, (Uint16)(length), SPI_WRITE);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("write(): SPI Write Failed\r\n");
    }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b read()
 *
 *   @b Description
 *   @n API to read an array of data from the SPI
 *
 *   @b Arguments
 *   @verbatim
        buffer [IN] Buffer that will holds the read data
        length [IN] Length of the Buffer to be read
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI read successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int SPI_Class::read (unsigned int *buffer, int length)
{
    CSL_Status status;

    if ((NULL == buffer) || (length <= 0))
    {
        SPI_LOG_MSG_PRINT("read(): Invalid Parameter\r\n");
        return ((int)CSL_ESYS_INVPARAMS);
    }

    status = SPI_dataTransaction(spiHandle, (Uint16 *)buffer, (Uint16)(length), SPI_READ);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("read(): SPI Read Failed\r\n");
    }
    // else
    // {
    //     swapWords (buffer, length);

    //     if (LSBFIRST == order)
    //     {
          //    If the User requests to receive LSB first, then swap all the 32
          //      bits of the User data 
    //         swap32Bits (buffer, length);
    //     }
    // }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b wordLength()
 *
 *   @b Description
 *   @n API to set the wordLength of SPI bus
 *
 *   @b Arguments
 *   @verbatim
        length [IN] spi word length in bits
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI read successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */

int SPI_Class::setWordLength(unsigned int length)
{
    CSL_Status status;

    SPI_WordLen len = (SPI_WordLen) length;
    if(len > SPI_MAX_WORD_LEN)
    {
        return CSL_FAIL;
    }

    spiConfig.wLen = len;

    status = SPI_config(spiHandle, &spiConfig);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setClockDivider(): setClockDivider() API Failed");
        SPI_LOG_MSG_PRINT ("\r\n");
    }
    return ((int) status);
}


/** ===========================================================================
 *   @n@b setDataDelay()
 *
 *   @b Description
 *   @n API to set the wordLength of SPI bus
 *
 *   @b Arguments
 *   @verbatim
        delay [IN] number of bits to delay 0-3
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SPI read successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */

int SPI_Class::setDataDelay(unsigned int delay)
{
    CSL_Status status;

    SPI_DataDly del = (SPI_DataDly) delay;
    if(del > SPI_DATA_DLY_UNKNOWN)
    {
        return CSL_FAIL;
    }

    spiConfig.dataDelay = del;

    status = SPI_config(spiHandle, &spiConfig);
    if(CSL_SOK != status)
    {
        SPI_LOG_MSG_PRINT ("setClockDivider(): setClockDivider() API Failed");
        SPI_LOG_MSG_PRINT ("\r\n");
    }
    return ((int) status);
}

/** ===========================================================================
 *   @n@b SPI_LOG_MSG_PRINT()
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
void SPI_LOG_MSG_PRINT (char *printString)
{
#ifdef ENABLE_SERIAL_MSGS
    Serial.print(printString);
#else
    printf("%s", printString);
#endif
}


#endif /* BOARD_DSPSHIELD_V2 */
