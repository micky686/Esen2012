#ifndef TTPA_ROSE_H
#define TTPA_ROSE_H
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
 * include/ttpa_rose.h
 *      Declarations for ROSE file format
 *
 * $Id: ttpa_rose.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/15 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/06 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added structs and macros for ttpa_rosefile
 *   2006/11/27 (0.6): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - fixed bug in TTPA_ROSESTATUS for sec3_start
 *
 */
#include <inttypes.h>

/*
 * ATTENTION: TTPA_ROSELEN_SEC2 and TTPA_ROSELEN_SEC3 must always be set 
 * according to the used ROSE file!
 */
#if !defined (TTPA_ROSELEN_SEC2)
#define TTPA_ROSELEN_SEC2 4
#endif /* TTPA_ROSELEN_SEC2 */
#if !defined (TTPA_ROSELEN_SEC3)
#define TTPA_ROSELEN_SEC3 4
#endif /* TTPA_ROSELEN_SEC3 */


typedef struct {
	uint8_t b[8];
} ttpa_time_t; 

typedef struct {
	uint8_t active_sec;
	uint8_t sec2_start;
	uint8_t sec3_start;
	uint8_t reserved;
} ttpa_rosestat_t;

#define TTPA_ROSESTATUS(active) \
.status = { .active_sec = 0x01 & active, .sec2_start = 2, \
.sec3_start = (sizeof(ttpa_time_t))/2 + TTPA_ROSELEN_SEC2 + 2}

typedef struct {
	union {
		struct {
			unsigned rndname : 3;
			unsigned : 4;
			unsigned eors : 1;
		};
		uint8_t round;
	};
	union {
		struct {
			unsigned irglen : 4;
			unsigned : 4;
		};
		uint8_t irg;
	};
	uint8_t reserved[2];
} ttpa_roseentry_t;

#define TTPA_ROSEENTRY(round, irg)      \
{ { { .rndname = round, .eors = 0 } }, \
  { { .irglen = irg } } }

#define TTPA_ROSEENTRY_EORS(round, irg)      \
{ { { .rndname = round, .eors = 1 } }, \
  { { .irglen = irg } } }

struct ttpa_rosefile_struct {
	ttpa_rosestat_t status;
	ttpa_time_t startt2;
	ttpa_time_t period2;
	ttpa_roseentry_t rose2[TTPA_ROSELEN_SEC2];
	ttpa_time_t startt3;
	ttpa_time_t period3;
	ttpa_roseentry_t rose3[TTPA_ROSELEN_SEC3];
};

extern struct ttpa_rosefile_struct ttpa_rosefile;

#define TTPA_ROSESEC2() \
.startt2 = { .b = { 0,0,0,0,0,0,0,0 } }, \
.period2 = { .b = { 0,0,0,0,0,0,0,0 } }, \
.rose2 =

#define TTPA_ROSESEC3() \
.startt3 = { .b = { 0,0,0,0,0,0,0,0 } }, \
.period3 = { .b = { 0,0,0,0,0,0,0,0 } }, \
.rose3 =

#define TTPA_ROSEFILE() \
struct ttpa_rosefile_struct __attribute__ ((section(".ifs_int_eep"))) ttpa_rosefile =

#endif /* TTPA_ROSE_H */
