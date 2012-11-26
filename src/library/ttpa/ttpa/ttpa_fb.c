/* Copyright (c) 2004, Christian Trödhandl
   All rights reserved.
 
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
 
   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */
 
/*
 * ttpa/ttpa_fb.c
 *      Fireworks bytes code array located in Flash ROM
 *
 * $Id: ttpa_fb.c,v 1.3 2007-01-18 19:18:46 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/17 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2005/04/29 (0.4): T. Mozgan <thomas.m@gmx.at>
 *    - fixed missing entry for RODL2
 *
 */
#include <avr/pgmspace.h>

#include "ttpa_fb.h"

const uint8_t __attribute__ ((progmem)) ttpa_fbcodes[] = {
	TTPA_FB_RODL0, TTPA_FB_MSD, TTPA_FB_RODL2, TTPA_FB_RODL3, 
	TTPA_FB_RODL4, TTPA_FB_MSA, TTPA_FB_RODL6, TTPA_FB_RODL7
};
