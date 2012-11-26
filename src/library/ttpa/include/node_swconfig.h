#ifndef NODE_SWCONF_H
#define NODE_SWCONF_H
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
 * include/node_swconf.h
 *      Macro for SW configurable nodes
 *
 * $Id: node_swconfig.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/03/05 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/12/10 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - include node_config.h
 *
 */
#include "node_config.h"

#define node_swconf_lowen(switch) do { \
	SWCONF_IOPORT &= ~(1<<switch); \
	SWCONF_IODDR |= (1<<switch);   \
} while (0)

#define node_swconf_highen(switch) do { \
	SWCONF_IOPORT |= (1<<switch); \
	SWCONF_IODDR |= (1<<switch);   \
} while (0)

#define node_swconf_null() do { } while(0)

#if !defined (node_swconf_swuart)
#define node_swconf_swuart() node_swconf_null() 
#endif /*  node_swconf_swuart */
#if !defined (node_swconf_hwuart)
#define node_swconf_hwuart() node_swconf_null() 
#endif /*  node_swconf_hwuart */
#if !defined (node_swconf_avcc_a)
#define node_swconf_avcc_a() node_swconf_null() 
#endif /*  node_swconf_avcc_a */
#if !defined (node_swconf_avcc_d)
#define node_swconf_avcc_d() node_swconf_null() 
#endif /*  node_swconf_avcc_d */
#if !defined (node_swconf_transcvr_isok)
#define node_swconf_transcvr_isok() node_swconf_null()  
#endif /*  node_swconf_avcc_d */
#if !defined (node_swconf_transcvr_rs485)
#define node_swconf_transcvr_rs485() node_swconf_null() 
#endif /* node_swconf_transcvr_rs485 */

#endif /* NODE_SWCONF_H */
