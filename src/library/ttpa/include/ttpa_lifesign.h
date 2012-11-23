#ifndef TTPA_LIFESIGN_H
#define TTPA_LIFESIGN_H
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
 * include/ttpa_lifesign.h
 *      shows protocol status on status LED
 *
 * $Id: ttpa_lifesign.h,v 1.3 2007-01-18 09:09:01 bernd Exp $
 *
 * Revision History:
 *   2004/08/04 (0.3): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed passive blinking pattern
 *
 */

#include "node_config.h"
#include "ttpa_sysfiles.h"

#define LS_MASK 0x80
#define LS_PASV 0x40

#if !( defined(LS_NONE) || defined(LS_SINGLE) || defined(LS_DOUBLE) || defined(LS_DUO))
#if defined(LS_LED0)
#if defined(LS_LED1)
#define LS_DOUBLE
#else
#define LS_SINGLE
#endif /* LS_LED1 */
#else
#define LS_NONE
#endif /* LS_LED0 */
#endif

#if defined(LS_LED0)
#define LS_INIT_0 \
do { \
	LS_DDR0 |= (1<<LS_LED0); \
} while(0)
#else
#define LS_INIT_0 do {} while(0)
#endif /* LS_LED0 */

#if defined(LS_LED1)
#define LS_INIT_1 \
do { \
	LS_DDR1 |= (1<<LS_LED1); \
} while(0)
#else
#define LS_INIT_1 do {} while(0)
#endif /* LS_LED1 */

#if defined(LS_INV)
#define LS_ON_0 \
do { \
	LS_PORT0 &= ~(1<<LS_LED0); \
} while(0)
#define LS_OFF_0 \
do { \
	LS_PORT0 |= (1<<LS_LED0); \
} while(0)
#if defined(LS_LED1)
#define LS_ON_1 \
do { \
	LS_PORT1 &= ~(1<<LS_LED1); \
} while(0)
#define LS_OFF_1 \
do { \
	LS_PORT1 |= (1<<LS_LED1); \
} while(0)
#else
#define LS_ON_1 do {} while(0)
#define LS_OFF_1 do {} while(0)
#endif /* LS_LED1 */
#else
#define LS_ON_0 \
do { \
	LS_PORT0 |= (1<<LS_LED0); \
} while(0)
#define LS_OFF_0 \
do { \
	LS_PORT0 &= ~(1<<LS_LED0); \
} while(0)
#if defined(LS_LED1)
#define LS_ON_1 \
do { \
	LS_PORT1 |= (1<<LS_LED1); \
} while(0)
#define LS_OFF_1 \
do { \
	LS_PORT1 &= ~(1<<LS_LED1); \
} while(0)
#else
#define LS_ON_1 do {} while(0)
#define LS_OFF_1 do {} while(0)
#endif /* LS_LED1 */
#endif /* LS_INV */

#if defined(LS_LED0)
#define LS_HALF_0 \
do { \
	LS_PORT0 ^=  (1<<LS_LED0); \
} while(0)
#else
#define LS_HALF_0 do {} while(0)
#endif /* LS_HALF_0 */

#if defined(LS_LED1)
#define LS_HALF_1 \
do { \
	LS_PORT1 ^=  (1<<LS_LED1); \
} while(0)
#else
#define LS_HALF_1 do {} while(0)
#endif /* LS_HALF_1 */

#if defined(LS_NONE)
#define ttpa_ls_init() do {} while(0)
#define ttpa_ls_passive() do {} while(0)
#define ttpa_ls_active() do {} while(0)
#define ttpa_ls_error() do {} while(0)
#elif defined(LS_SINGLE)
#define ttpa_ls_init() \
do { \
	LS_OFF_0; \
	LS_INIT_0; \
} while(0)
#define ttpa_ls_passive() \
do { \
	if((ttpa_conffile.ectr & LS_MASK)&&(ttpa_conffile.ectr & LS_PASV)) { \
		LS_HALF_0; \
	} else { \
		LS_OFF_0; \
	} \
} while(0)
#define ttpa_ls_active() \
do { \
	if(ttpa_conffile.ectr & LS_MASK) { \
		LS_ON_0; \
	} else { \
		LS_OFF_0; \
	} \
} while(0)
#define ttpa_ls_error() \
do { \
	LS_ON_0; \
} while(0)
#elif defined(LS_DOUBLE)
#define ttpa_ls_init() \
do { \
	LS_OFF_0; \
	LS_INIT_0; \
	LS_OFF_1; \
	LS_INIT_1; \
} while(0)
#define ttpa_ls_passive() \
do { \
	LS_OFF_1; \
	if((ttpa_conffile.ectr & LS_MASK)&&(ttpa_conffile.ectr & LS_PASV)) { \
		LS_HALF_0; \
	} else { \
		LS_OFF_0; \
	} \
} while(0)
#define ttpa_ls_active() \
do { \
	LS_OFF_1; \
	if(ttpa_conffile.ectr & LS_MASK) { \
		LS_ON_0; \
	} else { \
		LS_OFF_0; \
	} \
} while(0)
#define ttpa_ls_error() \
do { \
	LS_OFF_0; \
	LS_ON_1; \
} while(0)
#elif defined(LS_DUO)
#define ttpa_ls_init() \
do { \
	LS_OFF_0; \
	LS_INIT_0; \
	LS_OFF_1; \
	LS_INIT_1; \
} while(0)
#define ttpa_ls_passive() \
do { \
	if((ttpa_conffile.ectr & LS_MASK)&&(ttpa_conffile.ectr & LS_PASV)) { \
		LS_ON_0; \
		LS_ON_1; \
	} else { \
		LS_OFF_0; \
		LS_OFF_1; \
	} \
} while(0)
#define ttpa_ls_active() \
do { \
	LS_OFF_1; \
	if(ttpa_conffile.ectr & LS_MASK) { \
		LS_ON_0; \
	} else { \
		LS_OFF_0; \
	} \
} while(0)
#define ttpa_ls_error() \
do { \
	LS_OFF_0; \
	LS_ON_1; \
} while(0)
#endif /* LS_NONE */

extern void ttpa_lifesign(void);

#endif /* TTPA_LIFESIGN_H */
