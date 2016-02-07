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
/** @file McSPI.cpp
 *
 *  @brief McSPI implementation
 *
 */
 
#include <core.h>
#include <McSPI.h>

/** Defining the McSPI Class global Member */
CSL_McSpiObj McSPI_Class::mcspiObject;

/** Defining a global McSPI Class Object which can be used by the User
    Application */
McSPI_Class McSPI;

/** Macro to enable the Print messages to be displayed on the Serial */
//#define ENABLE_SERIAL_MSGS

#ifdef ENABLE_SERIAL_MSGS
#include <Serial_lib.h>
#endif

/** ===========================================================================
 *   @n@b McSPI_LOG_MSG_PRINT()
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
void McSPI_LOG_MSG_PRINT (char *printString);


/** ===========================================================================
 *   @n@b begin()
 *
 *   @b Description
 *   @n API to Initialize the McSPI Module
 *
 *   @b Arguments
 *   @verbatim
        opmode - Operating mode of the McSPI Module
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - McSPI init successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int McSPI_Class::begin(int opmode)
{
    CSL_Status       status;
    CSL_McSpiHandle  mcspiHandle;

    if ((McSPI_POLL_MODE != opmode) &&
        (McSPI_INTERRUPT_MODE != opmode) &&
        (McSPI_DMA_MODE != opmode))
    {
        McSPI_LOG_MSG_PRINT("begin(): Invalid Operating Mode\r\n");
        return ((int)CSL_ESYS_INVPARAMS);
    }

    mcspiHandle = &(this->mcspiObject);

    status = McSPI_init(0);
    if (CSL_SOK != status)
    {
        McSPI_LOG_MSG_PRINT("begin(): McSPI_init Failed\r\n");
    }
    else
    {
        mcspiHandle = McSPI_open((McSPI_OperMode)opmode, mcspiHandle, &status);
        if ((mcspiHandle == NULL) || (status != CSL_SOK))
        {
            McSPI_LOG_MSG_PRINT("begin(): McSPI_open() Failed\r\n");
        }
        else
        {
            status = SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE, CSL_EBSR_SP1MODE_1);
            if (CSL_SOK != status)
            {
                McSPI_LOG_MSG_PRINT("begin(): SYS_setEBSR failed\r\n");
            }
        }
    }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b end()
 *
 *   @b Description
 *   @n API to Close the McSPI Module
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - McSPI close successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int McSPI_Class::end()
{
    CSL_Status status;

    status = McSPI_close(&(this->mcspiObject));
    if (CSL_SOK != status)
    {
        McSPI_LOG_MSG_PRINT("end(): McSPI_close API failed\r\n");
    }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b config()
 *
 *   @b Description
 *   @n API to Configure the McSPI module
 *
 *   @b Arguments
 *   @verbatim
        chipSelect - McSPI Chip Select Number
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - McSPI close successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int McSPI_Class::config(int chipSelect)
{
    CSL_Status      status;
    CSL_McSpiHandle mcspiHandle;

    if ((McSPI_CHIPSELECT_0 < chipSelect) || (McSPI_CHIPSELECT_3 > chipSelect))
    {
        return ((int)CSL_ESYS_INVPARAMS);
    }

    mcspiHandle = &(this->mcspiObject);

    mcspiHandle->McSPIHwConfig.mode      = McSPI_MASTER;
    mcspiHandle->McSPIHwConfig.duplex    = McSPI_FULL_DUPLEX;
    mcspiHandle->McSPIHwConfig.wLen      = McSPI_WORD_LENGTH_8;
    mcspiHandle->McSPIHwConfig.dataDelay = McSPI_DATA_NODLY;
    mcspiHandle->McSPIHwConfig.ClkDiv    = CSL_MCSPI_CH0CONFL_CLKD_DIV1;
    mcspiHandle->McSPIHwConfig.clkPh     = McSPI_CLK_PH_EVEN_EDGE;
    mcspiHandle->McSPIHwConfig.clkPol    = McSPI_CLKP_LOW;
    mcspiHandle->ch                      = (McSPI_ch)chipSelect;
    mcspiHandle->McSPIHwConfig.csPol     = McSPI_CSP_ACTIVE_LOW;
    mcspiHandle->McSPIHwConfig.inSel     = McSPI_RX_SPIDAT0;
    mcspiHandle->McSPIHwConfig.enDat0_1  = McSPI_TX_ENABLE_SPIDAT0;

    status = McSPI_config(mcspiHandle);
    if (CSL_SOK != status)
    {
        McSPI_LOG_MSG_PRINT("config(): McSPI_Config API failed\r\n");
    }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b read()
 *
 *   @b Description
 *   @n API to Read data from a specified device
 *
 *   @b Arguments
 *   @verbatim
        readBuffer - Buffer to store the data read from the device
        bufferLen  - Length of the buffer to read
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - McSPI close successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int McSPI_Class::read(unsigned int *readBuffer, unsigned int bufferLen)
{
    CSL_Status status;

    if ((NULL == readBuffer) || (0 == bufferLen))
    {
        McSPI_LOG_MSG_PRINT("read(): Invalid Arguments\r\n");
        return ((int)CSL_ESYS_INVPARAMS);
    }

    status = McSPI_read(&(this->mcspiObject),
                        (Uint16 *)readBuffer,
                        (Uint16)bufferLen);
    if (CSL_SOK != status)
    {
        McSPI_LOG_MSG_PRINT("read(): McSPI_read API failed\r\n");
    }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b write()
 *
 *   @b Description
 *   @n API to Write data to a specified device
 *
 *   @b Arguments
 *   @verbatim
        writeBuffer - Buffer to be written to the device
        bufferLen   - Length of the buffer to write
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - McSPI close successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int McSPI_Class::write(unsigned int *writeBuffer, unsigned int bufferLen)
{
    CSL_Status status;

    if ((NULL == writeBuffer) || (0 == bufferLen))
    {
        McSPI_LOG_MSG_PRINT("write(): Invalid Arguments\r\n");
        return ((int)CSL_ESYS_INVPARAMS);
    }

    status = McSPI_write(&(this->mcspiObject),
                         (Uint16 *)writeBuffer,
                         (Uint16)bufferLen);
    if (CSL_SOK != status)
    {
        McSPI_LOG_MSG_PRINT("write(): McSPI_write API failed\r\n");
    }

    return ((int)status);
}

/** ===========================================================================
 *   @n@b isRxFull()
 *
 *   @b Description
 *   @n API to check whether the RX channel is full or not
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  TRUE  - If RX channel is full
 *   @li                  FALSE - If RX channel is not full
 *
 *  ===========================================================================
 */
Bool McSPI_Class::isRxFull()
{
    Bool retValue;

    retValue = (Bool)McSPI_RxFullInt_Chk(&(this->mcspiObject));

    return (retValue);
}

/** ===========================================================================
 *   @n@b isTxFull()
 *
 *   @b Description
 *   @n API to check whether the TX channel is empty or not
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  TRUE  - If TX channel is empty
 *   @li                  FALSE - If TX channel is not empty
 *
 *  ===========================================================================
 */
Bool McSPI_Class::isTxFull()
{
    Bool retValue;

    retValue = (Bool)McSPI_TxEmptyInt_Chk(&(this->mcspiObject));

    return (retValue);
}

/** ===========================================================================
 *   @n@b McSPI_LOG_MSG_PRINT()
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
void McSPI_LOG_MSG_PRINT (char *printString)
{
#ifdef ENABLE_SERIAL_MSGS
    Serial.print(printString);
#else
    printf("%s", printString);
#endif
}
