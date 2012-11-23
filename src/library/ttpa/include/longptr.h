#ifndef LONGPTR_H
#define LONGPTR_H
/* Copyright (c) 2005, Christian Trödhandl
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
 * include/longptr.h
 *      Return long pointer
 *
 * $Id: longptr.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2005/10/28 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added macro ptr_to_ui32()
 *
 */

#include <inttypes.h>

/**
 * \brief Return full length of a pointer.
 *
 * Stores the full length of a pointer (24 bit) into a 32 bit unsigned int.
 *
 */
#define ptr_to_ui32(ptr) \
({ \
	uint32_t res; \
	asm volatile("ldi %A0, lo8(" #ptr ")\n\t" \
		     "ldi %B0, hi8(" #ptr ")\n\t" \
		     "ldi %C0, hh8(" #ptr ")\n\t" \
		     "clr %D0\n\t" : "=r" (res) : ); \
	res; \
})

#endif /* LONGPTR_H */
