#ifndef TTPA_SYSFILES_H
#define TTPA_SYSFILES_H
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
 * include/ttpa_sysfiles.h
 *      Declarations for TTP/A system files
 *
 * $Id: ttpa_sysfiles.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/26 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/06 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added file no. for sysfiles
 */
#include "ifs_types.h"
#include "ifs_rodl.h"
#include "ttpa_task.h"

/* Definition of reserved node ID's */
#define LN_BROADC 0x00 // logical name used in broadcast
#define LN_RES1   0xfb // reserved logical name
#define LN_RES2   0xfc // reserved logical name
#define LN_GW     0xfd // logical name of gateway
#define LN_MASTER 0xfe // logical name of master
#define LN_NBAPT  0xff // logical name of unbaptized nodes
 
#ifndef LN_NODE
#if defined(MASTER)
#define LN_NODE LN_MASTER // node is master
#else
#define LN_NODE LN_NBAPT // node is not baptized and no ID has been compiled in
#endif /* MASTER */
#endif /* LN_NODE */

#ifndef CN_NODE
#define CN_NODE 0x00 // default cluster name
#endif /* CN_NODE */

#ifndef LN_DYN_START
#define LN_DYN_START 0x80
#endif /* LN_DYN_START */
#ifndef LN_DYN_END
#define LN_DYN_END 0xbf
#endif /* LN_DYN_END */

#ifndef SERIES_NR
#define SERIES_NR 0xffffffff
#endif /* SERIES_NR */
#ifndef SERIAL_NR
#define SERIAL_NR 0xffffffff
#endif /* SERIAL_NR */

#define TTPA_SYSFILE_CONF 0x08
#define TTPA_SYSFILE_MEMBERS 0x09
#define TTPA_SYSFILE_ROSE 0x0a
#define TTPA_SYSFILE_OWNER 0x0b
#define TTPA_SYSFILE_MSDATA 0x0c
#define TTPA_SYSFILE_UNUSED_0x0d 0x0d
#define TTPA_SYSFILE_UNUSED_0x0e 0x0e
#define TTPA_SYSFILE_UNUSED_0x0f 0x0f
#define TTPA_SYSFILE_DOC 0x3d
#define TTPA_SYSFILE_UNUSED_0x3e 0x3e
#define TTPA_SYSFILE_UNUSED_0x3f 0x3f


// ttpa_conf_master(): node is master
#if defined(MASTER) && !defined(SLAVE)
#define ttpa_conf_master() 1
#elif !defined(MASTER)
#define ttpa_conf_master() 0
#else
#define ttpa_conf_master() ((ttpa_conffile.cln == LN_MASTER) ? 1 : 0)
#endif

// Structure of Configuration File (0x08)
struct ttpa_conffile_struct {
	ifs_uint8_t stat;
	ifs_uint8_t ccn;
	ifs_uint8_t nln;
	ifs_uint8_t cln;
	union {
		struct {
			ifs_uint32_t bapt_series;
			ifs_uint32_t bapt_serial;
		};
		ifs_uint8_t idcv[8];
	};
	ifs_uint8_t cmux;
	ifs_uint8_t crnd;
	ifs_uint8_t ectr;
	ifs_uint8_t sctr;
#if defined(SUPPORT_SLEEP)
	ifs_uint8_t sleep[4];
#endif /* SUPPORT_SLEEP */
};

extern struct ttpa_conffile_struct ttpa_conffile;

extern void ttpa_conf_task(ttpa_taskparam_t param);

struct ttpa_docfile_struct {
	ifs_uint32_t series_nr;
	ifs_uint32_t serial_nr;
};

extern struct ttpa_docfile_struct ttpa_docfile;

#endif /* TTPA_SYSFILES_H */
