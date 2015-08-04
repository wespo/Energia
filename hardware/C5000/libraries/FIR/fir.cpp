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

/** @file fir.cpp
 *
 *  @brief Filter API implementation
 *
 */

#include "fir.h"

/**
 *
 * Wrapper API for the dsplib FIR filter API.
 *
 * int *input               <- Address of the Buffer containing Input
 *                             samples
 * int *coeffs              <- Address of the Buffer containing the
 *                             coefficients for FIR filter
 * int *output              <- Address of the Buffer to hold the output
 *                             samples
 * int *delayBuffer         <- Address of the Buffer to hold the delay
 *                             buffer of the FIR filter
 * unsigned int noOfSamples <- Length of the Input Samples Vector
 * unsigned int noOfCoeffs  <- Length of the Coefficient Vector
 *
 * Returns: int overFlow -> 0 : 32 bit Overflow hasn't occurred
 *                          1 : 32 bit Overflow has occurred
 */
void FIRClass::init(int *coeffsLoc, int coeffsLen)
{
  noOfCoeffs = coeffsLen;
  coeffs = coeffsLoc;
  delete[] delayBuffer;

  delayBuffer = new int [coeffsLen+2];
}

int FIRClass::filter(int *input, int *output, int len)
{
    int overFlow;
    overFlow = (int)fir((DATA*)input, (DATA*)coeffs,(DATA*)output, (DATA*)delayBuffer, len, noOfCoeffs);
    return (overFlow);
}