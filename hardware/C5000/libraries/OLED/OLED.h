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
/** @file OLED.h
 *
 *  @brief OLED library header file
 *
 */

#ifndef _OLED_H_
#define _OLED_H_

#include "core.h"
#include "bitmapDb.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief  Enumeration for number format
*/
typedef enum NUMBER_BASE
{
    BINARY,
    DECIMAL,
    OCTAL,
    HEXADECIMAL
} NUMBER_BASE;

/**
 * \brief OLED Class
 *
 *  Contains prototypes for functions in OLED library
 */
class OLED {
  int totalcols;
public:
  void oledInit(void);

  void init();

  void begin();

  void clear();
  void clear(int);
  int printchar(unsigned char);
  int print(char string[]);
  int print(long value);
  int print(char character);
  int print(long value, int base);
  void plot(int* vector, int len, bool bar);
  void write(unsigned char);

  void noDisplay();
  void display();
  void scrollDisplayLeft();
  void scrollDisplayLeft(int line);
  void scrollDisplayRight();
  void scrollDisplayRight(int line);

  void flip();
  void setOrientation(int newDir);
  void autoscroll();
  void noAutoscroll();

  void setline(int line);
  void setRolling(int row, int status);
  void resetCursor(int page);
  private:
  int row;
  int dir;
  int col[2];
  int rolling0;
  int rolling1;

  const Uint8 * FontBitmap;
  const FONT_CHAR_INFO * FontDescriptors;
  const FONT_INFO * FontInfo;
};

extern OLED disp;
/**< OLED class instance extern which can used by application programs
 *   to access OLED DSP APIs
 */
#ifdef __cplusplus
}
#endif

#endif //_OLED_H_
