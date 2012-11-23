#ifndef IFS_RODL_H
#define IFS_RODL_H
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
 * include/ifs_rodl.h
 *      Defines the RODL format
 *
 * $Id: ifs_rodl.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/21 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/06 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed IFS_RODLFILE()
 *     - added special RODL entries for mux reference
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added ifs_get_re_ptr()
 *
 */

#include <inttypes.h>
#include <stdio.h>

#include "ifs_types.h"
#include "ttpa_sysfiles.h"

typedef enum {
	RODL_OP_RECV,
	RODL_OP_SEND,
	RODL_OP_RECVSYNC,
	RODL_OP_EXEC,
	RODL_OP_RECV_SPDUP,
	RODL_OP_SEND_SPDUP,
	RODL_OP_EOR,
	RODL_OP_EXEC_EOR,
	RODL_OP_RECV_MUX4,
	RODL_OP_SEND_MUX4,
	RODL_OP_RECV_MUX8_0,
	RODL_OP_SEND_MUX8_0,
	RODL_OP_RECV_SPDUP_MUX4,
	RODL_OP_SEND_SPDUP_MUX4,
	RODL_OP_RECV_MUX8_1,
	RODL_OP_SEND_MUX8_1
} ifs_rodl_op_t;

// special OP codes used to mark invalid RODLs/slots
#define RODL_OP_IGNORE 0x10 // empty slot
#define RODL_OP_INVALID 0x80 // invalid RODL

typedef enum {
	SPDUP_4X,
	SPDUP_8X,
	SPDUP_16X,
	SPDUP_32X
} ifs_spdup_t;

typedef struct {
	union {
		struct {
			unsigned align_spdup : 2;
			unsigned filen : 6;
			uint8_t rec;
		};
		ifs_addr_t addr;
	} addr_u;
	unsigned f_len : 4;
	ifs_rodl_op_t op : 4;
	unsigned sl_pos : 6;
	unsigned mux : 2;
} ifs_rodlentry_t;

extern ifs_rodlentry_t *ifs_get_re_ptr(uint8_t round, uint8_t idx);

#define IFS_RE_RECV(re_sl_pos, re_addr, re_f_len)   \
{                                                   \
	.op = RODL_OP_RECV,                         \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_addr&0x03}}, \
        .f_len = re_f_len                           \
}

#define IFS_RE_SEND(re_sl_pos, re_addr, re_f_len)   \
{                                                   \
	.op = RODL_OP_SEND,                         \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_addr&0x03}}, \
	.f_len = re_f_len                           \
}

#define IFS_RE_RECVSYNC(re_sl_pos, re_addr, re_f_len) \
{                                                   \
	.op = RODL_OP_RECVSYNC,                     \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_addr&0x03}}, \
        .f_len = re_f_len                           \
}

#define IFS_RE_EXEC(re_sl_pos, re_addr)             \
{                                                   \
	.op = RODL_OP_EXEC,                         \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_addr&0x03}}, \
}

#define IFS_RE_RECV_SPDUP(re_sl_pos, re_addr, re_f_len, re_spdup) \
{                                                   \
	.op = RODL_OP_RECV_SPDUP,                   \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_spdup}},     \
        .f_len = re_f_len                           \
}

#define IFS_RE_SEND_SPDUP(re_sl_pos, re_addr, re_f_len, re_spdup) \
{                                                   \
	.op = RODL_OP_SEND_SPDUP,                   \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_spdup}},     \
        .f_len = re_f_len                           \
}

#define IFS_RE_EOR(re_sl_pos)                       \
{                                                   \
	.op = RODL_OP_EOR,                          \
        .sl_pos = re_sl_pos                         \
}

#define IFS_RE_EXEC_EOR(re_sl_pos, re_addr)         \
{                                                   \
	.op = RODL_OP_EXEC_EOR,                     \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_addr&0x03}}  \
}

#define IFS_RE_RECV_MUX4(re_sl_pos, re_addr, re_f_len, re_mux) \
{                                                   \
	.op = RODL_OP_RECV_MUX4,                    \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_addr&0x03}}, \
        .f_len = re_f_len,                          \
        .mux = re_mux                               \
}

#define IFS_RE_SEND_MUX4(re_sl_pos, re_addr, re_f_len, re_mux)   \
{                                                   \
	.op = RODL_OP_SEND_MUX4,                    \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_addr&0x03}}, \
        .f_len = re_f_len,                          \
        .mux = re_mux                               \
}

#define IFS_RE_RECV_MUX8(re_sl_pos, re_addr, re_f_len, re_mux) \
{                                                   \
	.op = RODL_OP_RECV_MUX8_0 | (re_mux & 0x04),\
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_addr&0x03}}, \
        .f_len = re_f_len,                          \
        .mux = re_mux & 0x03                        \
}

#define IFS_RE_SEND_MUX8(re_sl_pos, re_addr, re_f_len, re_mux) \
{                                                   \
	.op = RODL_OP_SEND_MUX8_0 | (re_mux & 0x04),\
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_addr&0x03}}, \
        .f_len = re_f_len,                          \
        .mux = re_mux & 0x03                        \
}

#define IFS_RE_RECV_SPDUP_MUX4(re_sl_pos, re_addr, re_f_len, re_spdup, re_mux) \
{                                                   \
	.op = RODL_OP_RECV_SPDUP_MUX4,              \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_spdup}},     \
        .f_len = re_f_len,                          \
        .mux = re_mux & 0x03                        \
}
#define IFS_RE_SEND_SPDUP_MUX4(re_sl_pos, re_addr, re_f_len, re_spdup, re_mux) \
{                                                   \
	.op = RODL_OP_SEND_SPDUP_MUX4,              \
        .sl_pos = re_sl_pos,                        \
	.addr_u = {{ .filen = (re_addr>>2)&0x3f,    \
                     .rec = (re_addr>>8)&0xff,      \
                     .align_spdup = re_spdup}},     \
        .f_len = re_f_len,                          \
        .mux = re_mux & 0x03                        \
}

// predefined RODL entries for mux reference

#define IFS_RE_MUX_MASTER(re_sl_pos) \
IFS_RE_SEND(re_sl_pos, IFS_ADDR_I(TTPA_SYSFILE_CONF,0x04,0x00), 0x00)

#define IFS_RE_MUX_SLAVE(re_sl_pos) \
IFS_RE_RECVSYNC(re_sl_pos, IFS_ADDR_I(TTPA_SYSFILE_CONF,0x04,0x00), 0x00)

#define IFS_RODLFILE(fn, len, loc) \
ifs_rodlentry_t __attribute__ ((section("." #loc))) ifs_rodlfile_ ## fn[len]; \
IFS_ADDRODLFILE(fn, ifs_rodlfile_ ## fn, NULL, len, loc, 006); \
ifs_rodlentry_t __attribute__ ((section("." #loc))) ifs_rodlfile_ ## fn[len] = 


#endif /* IFS_RODL_H */
