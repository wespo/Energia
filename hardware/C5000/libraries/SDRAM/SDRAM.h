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
/** @file SDRAM.h
 *
 *  @brief SDRAM library header file
 *
 */

#ifndef _SDRAM_H_
#define _SDRAM_H_

#include "tistdtypes.h"
#include "csl_emif.h"
#include "core.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Macro to indicate start address of SDRAM */
#define SDRAM_START_ADDR	(0x28000)

/**
 * \brief SDRAM Class
 *
 *  Contains prototypes for functions in SDRAM DSP API library
 */
class SDRAMClass {
    public:
	    int init(void);
	    int configure(void);
	    int write(unsigned short *data, unsigned long address, unsigned long length);
	    int read(unsigned short *data, unsigned long address, unsigned long length);
};

extern SDRAMClass SDRAM;
/**< SDRAM class instance extern which can be used by application programs
 *   to access SDRAM DSP APIs
 */

#ifdef __cplusplus
}
#endif

#endif //_SDRAM_H_
