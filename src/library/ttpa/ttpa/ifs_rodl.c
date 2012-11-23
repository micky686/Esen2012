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
 * ttpa/ifs_rodl.c
 *      RODL pointer lookup
 *
 * $Id: ifs_rodl.c,v 1.3 2007-01-18 09:09:01 bernd Exp $
 *
 * Revision History:
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - fast lookup of RODL pointer (ifs_get_re_ptr())
 *
 */

#include <stdio.h>
#include <avr/pgmspace.h>
#include "ifs.h"
#include "ifs_filetab.h"
#include "ifs_rodl.h"

ifs_rodlentry_t *ifs_get_re_ptr(uint8_t round, uint8_t idx)
{
	ifs_inodeptr_t iptr;
	ifs_rodlentry_t *ret;
	uint8_t len;

	iptr = (ifs_inodeptr_t) pgm_read_word(&(ifs_tab[round]));
	if(iptr == NULL) {
		return NULL;
	} else {
		ret = (ifs_rodlentry_t *) pgm_read_word(&(iptr->fptr));
		len = (uint8_t) pgm_read_byte(&(iptr->hrec.len));
		if(idx < len) {
			return ret + idx;
		} else {
			return NULL;
		}
	}
}
