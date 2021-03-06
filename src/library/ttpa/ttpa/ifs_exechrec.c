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
 * ttpa/ifs_exechrec.c
 *      execute of Header record: persistence handling, ...
 *
 * $Id: ifs_exechrec.c,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     TODO: - action for different alignments of header records need to be
 *             specified and implemented
 *
 */

#include "ifs_exechrec.h"
#include "ttpa_task.h"

/*
 * void ifs_exechrec(ttpa_taskparam_t param): execution of header record,
 * execute corresponding action
 */
void ifs_exechrec(ttpa_taskparam_t param)
{
	switch(param.align) {
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}
}
