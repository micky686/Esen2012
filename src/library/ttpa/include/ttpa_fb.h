#ifndef TTPA_FB_H
#define TTPA_FB_H
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
 * include/ttpa_fb.h
 *      Definition of the TTP/A fireworks bytes
 *
 * $Id: ttpa_fb.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/03/17 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *
 */
#include <inttypes.h>

#define TTPA_FB_RODL0 0x78
#define TTPA_FB_MSD   0x49
#define TTPA_FB_RODL2 0xba
#define TTPA_FB_RODL3 0x8b
#define TTPA_FB_RODL4 0x64
#define TTPA_FB_MSA   0x55
#define TTPA_FB_RODL6 0xa6
#define TTPA_FB_RODL7 0x97

extern const uint8_t ttpa_fbcodes[];

#define ttpa_getfb(round)                   \
({                                          \
	uint8_t __fb;                       \
        __fb = __LPM(&ttpa_fbcodes[round]); \
	__fb;                               \
})

#endif /* TTPA_FB_H */
