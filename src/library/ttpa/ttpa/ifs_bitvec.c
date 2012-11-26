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
 * ttpa/ifs_bitvec.c
 *     bitvector access 
 *
 * $Id: ifs_bitvec.c,v 1.2 2007-01-16 16:52:22 bernd Exp $
 *
 * Revision History:
 *   2004/05/12 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - macros to access bitvectors
 *     - ifs_bitvec_ffirst_cl()
 *
 */
#include <avr/pgmspace.h>
#include "ifs_bitvec.h"
#include "min_max.h"

uint8_t __attribute__ ((progmem)) bitvec_dec[8] =
{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

/*
 * ifs_bitvec_ffirst_cl(): find first cleared bit in bitvector
 */
uint16_t ifs_bitvec_ffirst_cl(ifs_bitvec_t *addr, uint16_t size, uint16_t start, uint16_t end)
{
	int i, j, lim;
	uint8_t msk;

	j = size - (start>>3) - 1;
	for(i = start; i <= end; i = (i+8)&0xf8) {
		if(~addr[j]) {
			for(lim = min(((i+8)&0xf8), end+1); i < lim; i++) {
				msk = __LPM(&bitvec_dec[i & 0x07]);
				if(!(msk&addr[j])) {
					return i;
				}
			}
			i--;
		}
		j--;
	}
	return end+1;
}
