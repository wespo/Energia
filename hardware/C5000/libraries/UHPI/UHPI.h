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
/** @file UHPI.h
 *
 *  @brief UHPI library header file
 *
 */

#ifndef _UHPI_H_
#define _UHPI_H_

#include <stdio.h>
#include "csl_uhpi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UHPI_GPIO_INTERRUPT_ENABLE    (1u)
/**< Macro indicating to enable GPIO interrupt for the UHPI   */
#define UHPI_GPIO_INTERRUPT_DISABLE    (0)
/**< Macro indicating to disable GPIO interrupt for the UHPI  */

/**
 * \brief UHPI Class
 *
 *  Contains prototypes for functions in UHPI DSP API library
 */
class UHPI_Class
{
    private:
        static CSL_UhpiObj uhpiObject;

    public:
        int begin();
        int end();
        int config(Bool gpioIntMode);
} ;

extern UHPI_Class UHPI;
/**< UHPI class instance extern which can be used by application programs
 *   to access UHPI DSP APIs
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _UHPI_H_ */
