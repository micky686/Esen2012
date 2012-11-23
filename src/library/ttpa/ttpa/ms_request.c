/* Copyright (c) 2004, 2005, Christian Trödhandl
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
 * $Id: ms_request.c,v 1.2 2007-01-16 16:52:22 bernd Exp $
 *
 * Revision History:
 *   2004/05/19 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added RODL entry caching
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed to support both, caching and non-caching version
 *
 */
#include "ttpa.h"
#include "ifs.h"
#include "ms_request.h"
#include "ttpa_sysfiles.h"
#include "ttpa_fb.h"

/*
 * ifs_err_t ms_request(ms_req_t *request, ms_data_t *data): executes a 
 * master - slave request
 * Returns 0 on success, and IFS error code on IFS error.
 */
ifs_err_t ms_request(ms_req_t *request, ms_data_t *data)
{
	ifs_fd_t fd;
	ifs_addr_t addr;
	uint8_t op;
	
//	addr.i = (request->fn_op & ~(OP_MSMSK)) | (request->rec<<8);
	addr.align = 0;
	addr.filen = request->fn_op>>2;
	addr.rec = request->rec;
	op = request->fn_op & OP_MSMSK;
	if(op == OP_MSRES) {
		return IFS_ERR_NOTSUPP;
	} else {
		ifs_open(&fd, addr, IFS_REQ(IFS_ACCESS_MP, op, 1));
	}
	if(op == OP_MSEXEC) {
		ifs_exec(&fd);
	} else if(op == OP_MSWRITE) {
		ifs_wr_blk(&fd, data->data, 0, 4);
#if defined(USE_RODLCACHE)
		if((addr.filen < 0x08) && (addr.rec == 1) && 
		                                (fd.error == IFS_ERR_SUCC )) {
			switch(ttpa_rodlentry_lookup(ttpa_re_cache+addr.filen,
					        (ifs_rodlentry_t *) fd.buf)) {
			case RE_LOOKUP_OK:
			case RE_LOOKUP_RODL:
				break;
			case RE_LOOKUP_MUTEX:
			case RE_LOOKUP_PARAM:
				ttpa_setstate(TTPA_STATE_ERROR);
				break;
			}
		}
#endif /* USE_RODLCACHE */
	} else {
		ifs_rd_blk(&fd, data->data, 0, 4);
		if(fd.error != IFS_ERR_SUCC) {
			data->data[0] = 0xff;
			data->data[1] = 0xff;
			data->data[2] = 0xff;
			data->data[3] = 0xff;
			data->check = fd.error | 0xf0;
		} else {
			data->check = ms_msd_check(data);
		}
	}
	return fd.error;
}
