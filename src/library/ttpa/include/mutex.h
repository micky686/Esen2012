#ifndef MUTEX_H
#define MUTEX_H
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
 * include/mutex.h
 *      inline functions for simple mutex variables
 *
 * $Id: mutex.h,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - first version
 */

#include <inttypes.h>

typedef uint8_t mutex_t;

static inline uint8_t mutex_test_set(volatile mutex_t *mutex)
{
	mutex_t ret;

	asm volatile("in __tmp_reg__,__SREG__\n\t"
		     "cli\n\t"
		     "ld %0,Z\n\t"
		     "out __SREG__,__tmp_reg__\n\t"
		     "st Z,%1\n\t"
		     : "=r" (ret) : "r" ((mutex_t) 1), "z" (mutex));
	return ret;
}

static inline void mutex_release(volatile mutex_t *mutex)
{
	*mutex = 0;
}

#endif /* MUTEX_H */
