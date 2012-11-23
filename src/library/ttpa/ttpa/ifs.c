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
 * ttpa/ifs.c
 *      general IFS functions
 *
 * $Id: ifs.c,v 1.4 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     TODO: - locking, ref counting (for EEPROM writes) for ifs_open and
 *             ifs_close
 *           - ifs_wr_xxx functions
 *           - legacy functions (ifs_read_xxx ifs_write_xxx)
 *   2004/05/18 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - improved ifs_rd_xxx functions
 *     - added ifs_fdok_rw(), ifs_rd_blk(), ifs_wr_blk()
 *   2004/05/18 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added ifs_wr_xxx() functions
 *
 */
#include <stdio.h>
#include <avr/pgmspace.h>
#include "ifs.h"
#include "ifs_filetab.h"
#include "ifs_exechrec.h"

/*
 * ifs_err_t ifs_open(ifs_fd_t *fd, ifs_addr_t addr, ifs_request_t req):
 * open an IFS file, return error code if operation fails
 */
ifs_err_t ifs_open(ifs_fd_t *fd, ifs_addr_t addr, ifs_request_t req)
{
	ifs_inodeptr_t iptr;
	ifs_inode_t inode;
	uint8_t mode;

	fd->op = req.op;
	iptr = (ifs_inodeptr_t) __LPM_word(&(ifs_tab[addr.filen]));
	if(iptr == NULL) {
		fd->error = IFS_ERR_NOFILE;
		return IFS_ERR_NOFILE;
	} else if(addr.rec == 0) {
		switch(req.op) {
		case IFS_OP_WRITE:
		case IFS_OP_RW:
			fd->error = IFS_ERR_RO;
			return IFS_ERR_RO;
		case IFS_OP_READ:
			fd->buf = iptr;
			fd->loc = IFS_LOC_ifs_flash;
			fd->len = 4;
			fd->error = IFS_ERR_SUCC;
			return IFS_ERR_SUCC;
		case IFS_OP_EXEC:
			fd->task = (ttpa_taskptr_t) ifs_exechrec;
			fd->addr = addr;
			fd->error = IFS_ERR_SUCC;
			return IFS_ERR_SUCC;
		}
	} else {
		memcpy_P(&inode, iptr, sizeof(ifs_inode_t));
		if(inode.hrec.pers == IFS_PERS_RO) {
			mode = 04;
		} else {
			mode = 06;
		}
		if(inode.task != NULL) {
			mode |= 01;
		}
		switch(req.acc) {
		case IFS_ACCESS_APPL:
			break;
		case IFS_ACCESS_MS:
			mode &= inode.hrec.mode_ms;
			break;
		case IFS_ACCESS_MP:
			mode &= inode.hrec.mode_mp;
			break;
		}
		switch(req.op) {
		case IFS_OP_READ:
			if(!(mode & 04)) {
				fd->error = IFS_ERR_NOTSUPP;
				return IFS_ERR_NOTSUPP;
			}
			break;
		case IFS_OP_RW:
			if(!(mode & 04)) {
				fd->error = IFS_ERR_NOTSUPP;
				return IFS_ERR_NOTSUPP;
			}				
		case IFS_OP_WRITE:
			if(!(mode & 02)) {
				fd->error = IFS_ERR_RO;
				return IFS_ERR_RO;
			}
			break;			
		case IFS_OP_EXEC:
			if(!(mode & 01)) {
				fd->error = IFS_ERR_NOEXEC;
				return IFS_ERR_NOEXEC;
			} else {
				fd->task = (ttpa_taskptr_t) inode.task;
				fd->addr = addr;
				fd->error = IFS_ERR_SUCC;
				return IFS_ERR_SUCC;
			}
			break;
		}
		if(addr.rec > inode.hrec.len) {
			fd->error = IFS_ERR_NOREC;
			return IFS_ERR_NOREC;
		}
		fd->buf = inode.fptr + 4*(addr.rec-1) + addr.align;
		fd->len = 4*(inode.hrec.len + 1 - addr.rec) - addr.align;
		fd->loc = inode.hrec.loc;
		// INSERT LOCATION SPECIFIC LOCKING / REF COUNTING
	    
		fd->noblk = req.noblk;
		fd->error = IFS_ERR_SUCC;
		return IFS_ERR_SUCC;
	}
	return IFS_ERR_NOTSUPP;
}

/*
 * ifs_err_t ifs_close(ifs_fd_t *fd): Close IFS file
 */
ifs_err_t ifs_close(ifs_fd_t *fd)
{
	return IFS_ERR_SUCC;
}

int ifs_fdok_rw(ifs_fd_t *fd, ifs_op_t op, uint16_t len)
{
	if(fd->error != IFS_ERR_SUCC) {
		return 0;
	}
	if((fd->op != op) && (fd->op != IFS_OP_RW)) {
		fd->error = IFS_ERR_NOTSUPP;
		return 0;
	}
	if(fd->len < len) {
		fd->error = IFS_ERR_NOREC;
		return 0;
	}
	return 1;
}

/*
 * uint8_t ifs_getc(ifs_fd_t *fd): Read a single byte from the IFS file and
 * increment fileptr
 */
uint8_t ifs_getc(ifs_fd_t *fd)
{
	if(ifs_fdok_rw(fd, IFS_OP_READ, 1)) {
		fd->len--;
		if(fd->loc == IFS_LOC_ifs_eep) {
			return ifs_inb_eep(fd->buf++);
		} else if(fd->loc == IFS_LOC_ifs_flash) {
			return ifs_inb_flash(fd->buf++);
		} else {
			return ifs_inb_mem(fd->buf++);
		}
	} else {
		return 0;
	}
}

/*
 * void ifs_putc(ifs_fd_t *fd, uint8_t byte): Write a single byte to the 
 * IFS file and increment fileptr
 */
void ifs_putc(ifs_fd_t *fd, uint8_t byte)
{
	if(ifs_fdok_rw(fd, IFS_OP_WRITE, 1)) {
		fd->len--;
		if(fd->loc == IFS_LOC_ifs_eep) {
			ifs_outb_eep(fd->buf++, byte);
		} else if(fd->loc == IFS_LOC_ifs_flash) {
			ifs_outb_flash(fd->buf++, byte);
		} else {
			ifs_outb_mem(fd->buf++, byte);
		}
	}
}

/*
 * uint8_t ifs_rd_u8(ifs_fd_t *fd, uint16_t offs): Read a byte from the IFS 
 * file on position offs
 */
uint8_t ifs_rd_u8(ifs_fd_t *fd, uint16_t offs)
{
	uint8_t result;

	if(ifs_fdok_rw(fd, IFS_OP_READ, offs + 1)) {
		if(fd->loc == IFS_LOC_ifs_eep) {
			result = ifs_inb_eep(fd->buf + offs);
		} else if(fd->loc == IFS_LOC_ifs_flash) {
			result = ifs_inb_flash(fd->buf + offs);
		} else {
			result = ifs_inb_mem(fd->buf + offs);
		}
		return result;
	} else {
		return 0;
	}
}

/*
 * uint16_t ifs_rd_u16(ifs_fd_t *fd, uint16_t offs): Read a word from the IFS 
 * file on position offs, change byteorder
 */
uint16_t ifs_rd_u16(ifs_fd_t *fd, uint16_t offs)
{
	union {
		uint16_t w;
		uint8_t b[2];
	} result;
	
	if(ifs_fdok_rw(fd, IFS_OP_READ, offs + 2)) {
		if(fd->loc == IFS_LOC_ifs_eep) {
			result.b[0] = (uint8_t) ifs_inb_eep(fd->buf+offs+1);
			result.b[1] = (uint8_t) ifs_inb_eep(fd->buf+offs);
		} else if(fd->loc == IFS_LOC_ifs_flash) {
			result.b[0] = (uint8_t) ifs_inb_flash(fd->buf+offs+1);
			result.b[1] = (uint8_t) ifs_inb_flash(fd->buf+offs);
		} else {
			result.b[0] = (uint8_t) ifs_inb_mem(fd->buf+offs+1);
			result.b[1] = (uint8_t) ifs_inb_mem(fd->buf+offs);
		}
		return result.w;
	} else {
		return 0;
	}
}

/*
 * uint32_t ifs_rd_u32(ifs_fd_t *fd, uint16_t offs): Read a long from the IFS 
 * file on position offs, change byteorder
 */
uint32_t ifs_rd_u32(ifs_fd_t *fd, uint16_t offs)
{
	union {
		uint32_t l;
		uint8_t b[4];
	} result;

	if(ifs_fdok_rw(fd, IFS_OP_READ, offs + 4)) {
		if(fd->loc == IFS_LOC_ifs_eep) {
			result.b[0] = (uint8_t) ifs_inb_eep(fd->buf+offs+3);
			result.b[1] = (uint8_t) ifs_inb_eep(fd->buf+offs+2);
			result.b[2] = (uint8_t) ifs_inb_eep(fd->buf+offs+1);
			result.b[3] = (uint8_t) ifs_inb_eep(fd->buf+offs);
		} else if(fd->loc == IFS_LOC_ifs_flash) {
			result.b[0] = (uint8_t) ifs_inb_flash(fd->buf+offs+3);
			result.b[1] = (uint8_t) ifs_inb_flash(fd->buf+offs+2);
			result.b[2] = (uint8_t) ifs_inb_flash(fd->buf+offs+1);
			result.b[3] = (uint8_t) ifs_inb_flash(fd->buf+offs);
		} else {
			result.b[0] = (uint8_t) ifs_inb_mem(fd->buf+offs+3);
			result.b[1] = (uint8_t) ifs_inb_mem(fd->buf+offs+2);
			result.b[2] = (uint8_t) ifs_inb_mem(fd->buf+offs+1);
			result.b[3] = (uint8_t) ifs_inb_mem(fd->buf+offs);
		}
		return result.l;
	} else {
		return 0;
	}
}

/*
 * ifs_rd_blk(): copies memory block from IFS to RAM (no byteorder 
 * conversion is performed!)
 */
ifs_err_t ifs_rd_blk(ifs_fd_t *fd, uint8_t *dest, uint16_t offs, uint16_t len)
{
	int i;
	if(ifs_fdok_rw(fd, IFS_OP_READ, offs + len)) {
		if(fd->loc == IFS_LOC_ifs_eep) {
			for(i = 0; i < len; i++) {
				dest[i] = ifs_inb_eep(fd->buf + offs + i);
			}
		} else if(fd->loc == IFS_LOC_ifs_flash) {
			for(i = 0; i < len; i++) {
				dest[i] = ifs_inb_flash(fd->buf + offs + i);
			}
		} else {
			for(i = 0; i < len; i++) {
				dest[i] = ifs_inb_mem(fd->buf + offs + i);
			}
		}
	}
	return fd->error;
}

/*
 * ifs_wr_blk(): copies memory block from RAM to IFS (no byteorder 
 * conversion is performed; copy to Flash or EEPRON is not supported)
 */
ifs_err_t ifs_wr_blk(ifs_fd_t *fd, uint8_t *src, uint16_t offs, uint16_t len)
{
	int i;
	if(ifs_fdok_rw(fd, IFS_OP_WRITE, offs + len)) {
		if((fd->loc == IFS_LOC_ifs_eep) | 
		   (fd->loc == IFS_LOC_ifs_flash)) {
			fd->error = IFS_ERR_NOTSUPP;
		} else {
			for(i = 0; i < len; i++) {
				ifs_outb_mem((fd->buf + offs + i), src[i]);
			}
		}
	}
	return fd->error;
}

/*
 * uint8_t ifs_wr_u8(ifs_fd_t *fd, uint16_t offs, uint8_t ui): Write a byte 
 * to the IFS file on position offs
 */
ifs_err_t ifs_wr_u8(ifs_fd_t *fd, uint16_t offs, uint8_t ui)
{
	if(ifs_fdok_rw(fd, IFS_OP_WRITE, offs + 1)) {
		if((fd->loc == IFS_LOC_ifs_eep) | 
		   (fd->loc == IFS_LOC_ifs_flash)) {
			fd->error = IFS_ERR_NOTSUPP;
		} else {
			ifs_outb_mem((fd->buf + offs), ui);
		}
	}
	return fd->error;
}

/*
 * uint8_t ifs_wr_u16(ifs_fd_t *fd, uint16_t offs, uint16_t ui): Write a int 
 * to the IFS file on position offs
 */
ifs_err_t ifs_wr_u16(ifs_fd_t *fd, uint16_t offs, uint16_t ui)
{
	if(ifs_fdok_rw(fd, IFS_OP_WRITE, offs + 1)) {
		if((fd->loc == IFS_LOC_ifs_eep) | 
		   (fd->loc == IFS_LOC_ifs_flash)) {
			fd->error = IFS_ERR_NOTSUPP;
		} else {
			ifs_outb_mem((fd->buf + offs),
				     ((uint8_t)(ui>>8)) & 0xff);
			ifs_outb_mem((fd->buf + offs + 1),
				     ((uint8_t)(ui)) & 0xff);
		}
	}
	return fd->error;
}

/*
 * uint8_t ifs_wr_u32(ifs_fd_t *fd, uint16_t offs, uint32_t ui): Write a long
 * to the IFS file on position offs
 */
ifs_err_t ifs_wr_u32(ifs_fd_t *fd, uint16_t offs, uint32_t ui)
{
	union {
		uint32_t l;
		uint8_t b[4];
	} result;

	result.l = ui;
	if(ifs_fdok_rw(fd, IFS_OP_WRITE, offs + 1)) {
		if((fd->loc == IFS_LOC_ifs_eep) | 
		   (fd->loc == IFS_LOC_ifs_flash)) {
			fd->error = IFS_ERR_NOTSUPP;
		} else {
			ifs_outb_mem((fd->buf + offs), result.b[3]);
			ifs_outb_mem((fd->buf + offs + 1),  result.b[2]);
			ifs_outb_mem((fd->buf + offs + 2),  result.b[1]);
			ifs_outb_mem((fd->buf + offs + 3),  result.b[0]);
		}
	}
	return fd->error;
}
