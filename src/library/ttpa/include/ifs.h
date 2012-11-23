#ifndef IFS_H
#define IFS_H
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
 * include/ifs.h
 *      Declarations for global IFS functions and datastructures
 *
 * $Id: ifs.h,v 1.4 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/22 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/18 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - removed ; on end of IFS_ADDFILE()
 *     - added ifs_fdok_rw(), ifs_rd_blk(), ifs_wr_blk()
 *   2004/06/07 (0.3): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - fixed bug in ifs_rd_i16 and ifs_rd_i32
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added ifs_wr_xxx() macros
 *
 */
#include <avr/pgmspace.h>

#include "ttpa_task.h"
#include "ifs_types.h"
#include "ifs_memsect.h"

typedef enum {
	IFS_PERS_NONE,
	IFS_PERS_RW,
	IFS_PERS_RO,
	IFS_PERS_RES
} ifs_pers_t;

typedef enum {
	IFS_ERR_SUCC,     // no error
	IFS_ERR_NOCLSTR,  // cluster does't exist
	IFS_ERR_NONODE,   // node does't exist
	IFS_ERR_NOFILE,   // file does't exist
	IFS_ERR_NOREC,    // record does't exist / requested length to long
	IFS_ERR_DAMMGD,   // file dammaged
	IFS_ERR_NOTREADY, // node wasn't able to fullfill the op. in the
	                  // given time
	IFS_ERR_NOEXEC,   // not executeable
	IFS_ERR_RO,       // File is read only (e.g. ROM)
	IFS_ERR_NOMSG,    // addressed node didn't respond  
	IFS_ERR_COMM,     // communication (CRC) error
	IFS_ERR_TIME,     // timing error
	IFS_ERR_NOTSUPP   // Operation is not supported
} ifs_err_t;

typedef struct {
	union {
		struct {
			unsigned mode_ms : 3;
			unsigned mode_mp : 3;
			ifs_pers_t pers : 2;
		};
		uint8_t mode;
	};
	uint8_t len;
	union {
		struct {		
			unsigned loc : 4;
			unsigned res1 : 4;
		};
		uint8_t loc_b;
	};
	uint8_t res2;
} ifs_hrec_t;

typedef struct {
	ifs_hrec_t hrec;
	void *fptr;
	ttpa_taskptr_t task;
} ifs_inode_t;

typedef ifs_inode_t * ifs_inodeptr_t;

typedef enum {
	IFS_OP_WRITE,
	IFS_OP_READ,
	IFS_OP_RW,
	IFS_OP_EXEC
} ifs_op_t;

typedef enum {
	IFS_ACCESS_APPL,
	IFS_ACCESS_MS,
	IFS_ACCESS_MP
} ifs_access_t;

#ifdef OPTIMIZE_SRAM
typedef struct {
	union {
		struct {
			void *buf;
			uint16_t len;
		};
		struct {
			ttpa_taskptr_t task;
			ifs_addr_t addr;
		};
			
	};
	uint8_t error;
	unsigned loc : 4;
	ifs_op_t op : 2;
	unsigned noblk : 1;
} ifs_fd_t;
#else
typedef struct {
	union {
		struct {
			void *buf;
			uint16_t len;
		};
		struct {
			ttpa_taskptr_t task;
			ifs_addr_t addr;
		};
			
	};
	uint8_t error;
	uint8_t loc;
	ifs_op_t op : 8;
	uint8_t noblk;
} ifs_fd_t;
#endif /* OPTIMIZE_SRAM */

typedef union {
	struct {
		ifs_access_t acc : 2;
		ifs_op_t op : 2;
		unsigned noblk : 1;
	};
	uint16_t i;
} ifs_request_t;

#define IFS_REQ(r_acc, r_op, r_noblk) \
({                                    \
	ifs_request_t __res = {{      \
		.acc = r_acc,         \
		.op = r_op,           \
		.noblk = r_noblk      \
	}};                           \
        __res;                        \
})

extern ifs_err_t ifs_open(ifs_fd_t *fd, ifs_addr_t addr, ifs_request_t req);

extern ifs_err_t ifs_close(ifs_fd_t *fd);

extern int ifs_fdok_rw(ifs_fd_t *fd, ifs_op_t op, uint16_t len);

extern uint8_t ifs_rd_u8(ifs_fd_t *fd, uint16_t offs);

extern uint16_t ifs_rd_u16(ifs_fd_t *fd, uint16_t offs);

extern uint32_t ifs_rd_u32(ifs_fd_t *fd, uint16_t offs);

extern ifs_err_t ifs_rd_blk(ifs_fd_t *fd, uint8_t *dest, uint16_t offs, uint16_t len);

extern ifs_err_t ifs_wr_blk(ifs_fd_t *fd, uint8_t *src, uint16_t offs, uint16_t len);

extern ifs_err_t ifs_wr_u8(ifs_fd_t *fd, uint16_t offs, uint8_t ui);

extern ifs_err_t ifs_wr_u16(ifs_fd_t *fd, uint16_t offs, uint16_t ui);

extern ifs_err_t ifs_wr_u32(ifs_fd_t *fd, uint16_t offs, uint32_t ui);

#define ifs_exec(fd) \
do { \
	ttpa_taskptr_t __task = ((ifs_fd_t *) fd)->task; \
	ttpa_taskparam_t __param; \
	__param.rec = ((ifs_fd_t *) fd)->addr.rec; \
	__param.align = ((ifs_fd_t *) fd)->addr.align; \
	(* __task) (__param); \
} while(0)

#define ifs_rd_i8(fd, addr)                    \
({                                             \
	int8_t __res;                          \
	__res = (int8_t) ifs_rd_u8(fd, addr);  \
	__res;                                 \
})

#define ifs_rd_i16(fd, addr)                    \
({                                              \
	int16_t __res;                          \
	__res = (int16_t) ifs_rd_u16(fd, addr); \
	__res;                                  \
})

#define ifs_rd_i32(fd, addr)                    \
({                                              \
	int32_t __res;                          \
	__res = (int32_t) ifs_rd_u32(fd, addr); \
	__res;                                  \
})

#define ifs_rd_f(fd, addr)                      \
({                                              \
	float __res;                            \
        uint32_t __u32                          \
	__u32 = ifs_rd_u32(fd, addr);           \
        __asm__                                 \
        ("" : "=r" (__res) : "0" (__u32);       \
	__res;                                  \
})

#define ifs_wr_i8(fd, addr, ui)                    \
({                                                 \
	ifs_err_t  __err;                          \
	__err = ifs_wr_u8(fd, addr, (int8_t) ui);  \
	__err;                                     \
})

#define ifs_wr_i16(fd, addr, ui)                     \
({                                                   \
	ifs_err_t  __err;                            \
	__err = ifs_wr_u16(fd, addr, (int16_t) ui);  \
	__err;                                       \
})

#define ifs_wr_i32(fd, addr, ui)                     \
({                                                   \
	ifs_err_t  __err;                            \
	__err = ifs_wr_u32(fd, addr, (int32_t) ui);  \
	__err;                                       \
})

#define ifs_wr_f(fd, addr, ui))                \
({                                             \
	ifs_err_t  __err;                      \
	float __f32;                           \
        uint32_t __u32                         \
        __f32 = (float) ui;                    \
        __asm__                                \
        ("" : "=r" (__u32) : "0" (__f32);      \
	__err = ifs_wr_u32(fd, addr, __u32);   \
	__err;                                 \
})

extern uint8_t ifs_getc(ifs_fd_t *fd);

extern void ifs_putc(ifs_fd_t *fd, uint8_t byte);

#define IFS_ADDFILE(p_type, p_fn, p_file, p_task, p_len, p_loc, p_perm) \
        IFS_ADDFILE_REAL(p_type, p_fn, p_file, p_task, p_len, p_loc, p_perm)

#define IFS_ADDFILE_REAL(p_type, p_fn, p_file, p_task, p_len, p_loc, p_perm) \
ifs_inode_t __attribute__ ((section(".ttpa_ifshrec"))) \
ifs_##p_type##_##p_fn = {                              \
	{                                              \
		{ .mode = (uint8_t) (0x3f & p_perm)    \
		  | ((IFS_PERS_##p_loc<<6) & 0xc0) },  \
		.len = p_len,                          \
		{ { .loc = IFS_LOC_##p_loc } }         \
	},                                             \
	.fptr = p_file,                                \
	.task = p_task                                 \
}

#define IFS_LOC(loc) IFS_LOC_REAL(loc)

#define IFS_LOC_REAL(loc) __attribute__ ((section("."  #loc)))

#define IFS_FILELEN(p_struct) ((sizeof(p_struct)+3)>>2)

#define IFS_ADDRODLFILE(fn, file, task, len, loc, perm) \
IFS_ADDFILE(rodl, fn, file, task, len, loc, perm)

#define IFS_ADDSYSFILE(fn, file, task, len, loc, perm) \
IFS_ADDFILE(sys, fn, file, task, len, loc, perm)

#define IFS_ADDAPPLFILE(fn, file, task, len, loc, perm) \
IFS_ADDFILE(appl, fn, file, task, len, loc, perm)

#endif /* IFS_H */
