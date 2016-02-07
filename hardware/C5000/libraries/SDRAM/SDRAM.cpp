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
/** @file SDRAM.cpp
 *
 *  @brief SDRAM implementation
 *
 */

#include "SDRAM.h"

/** EMIF object instance which will be used by CSL functions             */
CSL_EmifObj       emifObj;

/** Pointer to EMIF CSL object                                           */
CSL_EmifHandle    hEmif;

/** Class identifier declaration                                         */
SDRAMClass SDRAM;

/** ===========================================================================
 *   @n@b init()
 *
 *   @b Description
 *   @n Function to initialize SDRAM Module. This is the first function to be
 *      called to use SDRAM DSP API library services
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SDRAM init successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int SDRAMClass::init(void)
{
	int status;

	hEmif = (CSL_EmifHandle)NULL;

	/* SDRAM is interfaced through EMIF, initialize EMIF */
	status = EMIF_init(&emifObj);
	if(status == CSL_SOK)
	{
		hEmif = &emifObj;

		status = SDRAM.configure();
	}

	return(status);
}

/** ===========================================================================
 *   @n@b configure()
 *
 *   @b Description
 *   @n Function to configure SDRAM
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                  CSL_SOK            - SDRAM configuration successful
 *   @li                  CSL_ESYS_INVPARAMS - Invalid parameter
 *   @li                  CSL_ESYS_BADHANDLE - Invalid handle
 *
 *  ===========================================================================
 */
int SDRAMClass::configure(void)
{
	CSL_SdramCfg     sdramCfg;
	CSL_SdramTimr    sdramTimr;
	int              status;

	/* Assign values to sdram config structure */
	sdramCfg.NM           = CSL_SDRAM_SDCR1_NM_DEFAULT;
	sdramCfg.CasLatency   = CSL_SDRAM_SDCR1_CL_DEFAULT;
	sdramCfg.bit11to9Lock = CSL_SDRAM_SDCR1_BIT_11_9_LOCK_DEFAULT;
	sdramCfg.ibankPos     = CSL_SDRAM_SDCR1_IBANK_DEFAULT;
	sdramCfg.ebank        = CSL_SDRAM_SDCR1_EBANK_DEFAULT;
	sdramCfg.pageSize     = CSL_SDRAM_SDCR1_PAGESIZE_DEFAULT;
	sdramCfg.selfRefresh  = CSL_SDRAM_SDCR2_SR_DEFAULT;
	sdramCfg.pdMode       = CSL_SDRAM_SDCR2_PD_DEFAULT;
	sdramCfg.pdwr         = CSL_SDRAM_SDCR2_PDWR_DEFAULT;
	sdramCfg.pasr         = CSL_SDRAM_SDCR2_PASR_DEFAULT;
	sdramCfg.rowSize      = CSL_SDRAM_ROW_ADDR_BITS_9;
	sdramCfg.ibankPos     = CSL_SDRAM_SDCR2_IBANK_POS_DEFAULT;
	sdramCfg.sdramDrive   = CSL_SDRAM_SDCR2_SDRAM_DRIVE_DEFAULT;
	sdramCfg.bit9to1Lock  = CSL_SDRAM_SDCR2_BIT_9_1_LOCK_DEFAULT;

	sdramTimr.tRAS        = CSL_SDRAM_TRAS_VAL;
	sdramTimr.tRC         = CSL_SDRAM_TRC_VAL;
	sdramTimr.tRRD        = CSL_SDRAM_TRRD_VAL;
	sdramTimr.tRFC        = CSL_SDRAM_TRFC_VAL;
	sdramTimr.tRP         = CSL_SDRAM_TRP_VAL;
	sdramTimr.tRCD        = CSL_SDRAM_TRCD_VAL;
	sdramTimr.tWR         = CSL_SDRAM_TWR_VAL;
	sdramTimr.tXS         = CSL_SDRAM_TXS_VAL;
	sdramTimr.refRate     = CSL_SDRAM_REFRATE_VAL;

	/* Configure SDRAM settings */
	status = SDRAM_config(hEmif, &sdramCfg, &sdramTimr);

	return(status);
}

/** ===========================================================================
 *   @n@b write()
 *
 *   @b Description
 *   @n Function to write data to SDRAM
 *
 *   @b Arguments
 *   @verbatim
 		data    - Data buffer to write
 		address - Start address of the SDRAM to write data
		length  - Length of data to be written to SDRAM in words
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                    CSL_SOK            - SDRAM write is successful
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int SDRAMClass::write(unsigned short *data, unsigned long address, unsigned long length)
{
	int status;

	status = CSL_ESYS_INVPARAMS;

	if((data != NULL) && (length != 0))
	{
		status = SDRAM_write(hEmif, length, address, data);
	}

	return status;
}

/** ===========================================================================
 *   @n@b read()
 *
 *   @b Description
 *   @n Function to read data from SDRAM
 *
 *   @b Arguments
 *   @verbatim
 		data    - Buffer to fill with the data read from SDRAM
 		address - Start address of the SDRAM to read data
		length  - Length of data to be read from SDRAM in words
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li                    CSL_SOK            - SDRAM read is successful
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS - Invalid parameters
 *
 *  ===========================================================================
 */
int SDRAMClass::read(unsigned short *data, unsigned long address, unsigned long length)
{
	int status;

	status = CSL_ESYS_INVPARAMS;

	if((data != NULL) && (length != 0))
	{
		status = SDRAM_read(hEmif, length, address, data);
	}

	return (status);
}
