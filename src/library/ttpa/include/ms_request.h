#ifndef MS_REQUEST_H
#define MS_REQUEST_H
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
 * include/ms_request.h
 *      Declarations for MS request datastructure and function
 *
 * $Id: ms_request.h,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   2004/05/19 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *
 */
#include "ifs_types.h"
#include "ttpa_fb.h"

#define OP_MSWRITE 0x00 // write data to slave IFS
#define OP_MSREAD  0x01 // read data from slave IFS
#define OP_MSRES   0x02 // reserved
#define OP_MSEXEC  0x03 // execute
#define OP_MSMSK   0x03 // mask for MS op.
 
typedef struct {
	ifs_uint8_t ln;
	ifs_uint8_t fn_op;
	ifs_uint8_t rec;
	ifs_uint8_t check;
} ms_req_t;	

typedef struct {
	ifs_uint8_t cluster;
	ms_req_t msreq;
} gw_req_t;

typedef struct {
	ifs_uint8_t data[4];
	ifs_uint8_t check;
} ms_data_t;

ifs_err_t ms_request(ms_req_t *request, ms_data_t *data);

#define ms_msa_check(request) \
({ \
	ms_req_t *__req = (ms_req_t *) request; \
	ifs_uint8_t __ret; \
	__ret = __req->ln ^ __req->fn_op ^ __req->rec ^ TTPA_FB_MSA; \
	__ret; \
})

#define ms_msd_check(buf) \
({ \
	ms_data_t *__buf = (ms_data_t *) buf; \
	ifs_uint8_t __ret; \
	__ret = __buf->data[0] ^ __buf->data[1] ^ \
		__buf->data[2] ^ __buf->data[3] ^ TTPA_FB_MSD; \
	__ret; \
})

#endif /* MS_REQUEST_H */
