#ifndef IFS_TYPES_H
#define IFS_TYPES_H
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
 * include/ifs_types.h
 *      Defines types and macros used with IFS files
 *
 * $Id: ifs_types.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/21 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/05 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed the byteorder conversion macros
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added IFS_ADDR_S()
 *
 */
#include <inttypes.h>

typedef uint8_t ifs_uint8_t;

typedef uint16_t ifs_uint16_t;

typedef uint32_t ifs_uint32_t;

typedef int8_t ifs_int8_t;

typedef int16_t ifs_int16_t;

typedef int32_t ifs_int32_t;

typedef float ifs_float32_t;

/*
   The following macros are used to convert from node 
   byteorder (little endian) to IFS byteorder (big endian)
*/

#define IFS_CONVBYTEORDER_L(intype, outtype, input) \
({                                                  \
	intype __param = (intype) input;            \
	outtype __result;                           \
	__asm__ volatile(                           \
                "mov %A0, %D1" "\n\t"               \
		"mov %B0, %C1" "\n\t"               \
		"mov %C0, %B1" "\n\t"               \
		"mov %D0, %A1" "\n\t"               \
		: "=&r" (__result)                  \
		: "r" (__param)                     \
	);                                          \
	__result;                                   \
})

#define IFS_CONVBYTEORDER_W(intype, outtype, input) \
({                                                  \
	intype __param = (intype) input;            \
	outtype __result;                           \
	__asm__ volatile(                           \
                "mov %A0, %B1" "\n\t"               \
		"mov %B0, %A1" "\n\t"               \
		: "=&r" (__result)                  \
		: "r" (__param)                     \
	);                                          \
	__result;                                   \
})

#define IFS_TO_UINT8(input) (uint8_t) (input)

#define IFS_TO_UINT16(input) IFS_CONVBYTEORDER_W(ifs_uint16_t, uint16_t, input)

#define IFS_TO_UINT32(input) IFS_CONVBYTEORDER_L(ifs_uint32_t, uint32_t, input)

#define IFS_TO_INT8(input) (int8_t) (input)

#define IFS_TO_INT16(input) IFS_CONVBYTEORDER_W(ifs_int16_t, int16_t, input)

#define IFS_TO_INT32(input) IFS_CONVBYTEORDER_L(ifs_int32_t, int32_t, input)

#define IFS_TO_FLOAT32(input) IFS_CONVBYTEORDER_L(ifs_float32_t, float, input)

#define UINT8_TO_IFS(input) (ifs_uint8_t) (input)

#define UINT16_TO_IFS(input) IFS_CONVBYTEORDER_W(uint16_t, ifs_uint16_t, input)

#define UINT32_TO_IFS(input) IFS_CONVBYTEORDER_L(uint32_t, ifs_uint32_t, input)

#define INT8_TO_IFS(input) (ifs_int8_t) (input)

#define INT16_TO_IFS(input) IFS_CONVBYTEORDER_W(int16_t, ifs_int16_t, input)

#define INT32_TO_IFS(input) IFS_CONVBYTEORDER_L(int32_t, ifs_int32_t, input)

#define FLOAT32_TO_IFS(input) IFS_CONVBYTEORDER_L(float, ifs_float32_t, input)

typedef union {
	struct {
		unsigned align : 2;
		unsigned filen : 6;
		uint8_t rec;
	};
	uint16_t i;
} ifs_addr_t;

#define IFS_ADDR(f_filen, f_rec, f_align) \
({                                        \
        ifs_addr_t addr;                  \
        addr.filen = f_filen;             \
        addr.rec = f_rec;                 \
        addr.align = f_align;             \
        addr;                             \
})

#define IFS_ADDR_I(f_filen, f_rec, f_align) \
(((f_filen<<2)&0x00fc) | ((f_rec<<8)&0xff00) | (f_align&0x0003))
	
#define IFS_ADDR_S(f_filen, f_rec, f_align) \
{ { f_align, f_filen, f_rec} }
	
#define IFS_UINT8_I(uint8) (ifs_uint8_t) (uint8)
 
#define IFS_UINT16_I(uint16) (ifs_uint16_t) (uint16)

#define IFS_UINT32_I(uint32) (ifs_uint32_t) (uint32)

#define IFS_INT8_I(int8) (ifs_int8_t) (int8)
 
#define IFS_INT16_I(int16) (ifs_int16_t) (int16)

#define IFS_INT32_I(int32) (ifs_int32_t) (int32)

#define IFS_FLOAT32_I(float32) (ifs_float32_t) (float32)

#endif /* IFS_TYPES_H */
