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
/** @file McSPI.h
 *
 *  @brief McSPI library header file
 *
 */

#ifndef _McSPI_H_
#define _McSPI_H_

#include <stdio.h>
#include "csl_mcspi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define McSPI_POLL_MODE      (0)
/**< Macro to indicate the Operating mode of McSPI as Polled Mode    */
#define McSPI_INTERRUPT_MODE (1)
/**< Macro to indicate the Operating mode of McSPI as Interrupt Mode */
#define McSPI_DMA_MODE       (2)
/**< Macro to indicate the Operating mode of McSPI as DMA Mode       */

#define McSPI_CHIPSELECT_0 (0)
/**< Macro to indicate the McSPI Chip Select Number as 0 */
#define McSPI_CHIPSELECT_1 (1)
/**< Macro to indicate the McSPI Chip Select Number as 1 */
#define McSPI_CHIPSELECT_2 (2)
/**< Macro to indicate the McSPI Chip Select Number as 2 */
#define McSPI_CHIPSELECT_3 (3)
/**< Macro to indicate the McSPI Chip Select Number as 3 */

/**
 * \brief McSPI Class
 *
 *  Contains prototypes for functions in McSPI library
 */
class McSPI_Class
{
    private:
        static CSL_McSpiObj mcspiObject;

    public:
        int begin(int opmode);
        int end();
        int config(int chipSelect);
        int read(unsigned int readBuffer[], unsigned int bufferLen);
        int write(unsigned int writeBuffer[], unsigned int bufferLen);
        Bool isRxFull();
        Bool isTxFull();
} ;

extern McSPI_Class McSPI;
/**< McSPI class instance extern which can used by application programs
 *   to access McSPI DSP APIs
 */
#ifdef __cplusplus
}
#endif

#endif /* #ifndef _McSPI_H_ */
