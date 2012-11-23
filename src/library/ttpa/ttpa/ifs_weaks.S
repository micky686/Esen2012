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
 * ttpa/ifs_filetab.S
 *	weak Symbols for IFS filetable
 *
 * $Id: ifs_weaks.S,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *	2004/04/20: First version, C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *
 */

.macro	ifs_weak_entry label
	.weak	\label
	.data
	.set	\label, 0
	.type	\label, @object
	.endm

.section .data
	ifs_weak_entry ifs_rodl_0x00
	ifs_weak_entry ifs_rodl_0x01
	ifs_weak_entry ifs_rodl_0x02
	ifs_weak_entry ifs_rodl_0x03
	ifs_weak_entry ifs_rodl_0x04
	ifs_weak_entry ifs_rodl_0x05
	ifs_weak_entry ifs_rodl_0x06
	ifs_weak_entry ifs_rodl_0x07
	ifs_weak_entry ifs_sys_0x08
	ifs_weak_entry ifs_sys_0x09
	ifs_weak_entry ifs_sys_0x0a
	ifs_weak_entry ifs_sys_0x0b
	ifs_weak_entry ifs_sys_0x0c
	ifs_weak_entry ifs_sys_0x0d
	ifs_weak_entry ifs_sys_0x0e
	ifs_weak_entry ifs_sys_0x0f
	ifs_weak_entry ifs_appl_0x10
	ifs_weak_entry ifs_appl_0x11
	ifs_weak_entry ifs_appl_0x12
	ifs_weak_entry ifs_appl_0x13
	ifs_weak_entry ifs_appl_0x14
	ifs_weak_entry ifs_appl_0x15
	ifs_weak_entry ifs_appl_0x16
	ifs_weak_entry ifs_appl_0x17
	ifs_weak_entry ifs_appl_0x18
	ifs_weak_entry ifs_appl_0x19
	ifs_weak_entry ifs_appl_0x1a
	ifs_weak_entry ifs_appl_0x1b
	ifs_weak_entry ifs_appl_0x1c
	ifs_weak_entry ifs_appl_0x1d
	ifs_weak_entry ifs_appl_0x1e
	ifs_weak_entry ifs_appl_0x1f
	ifs_weak_entry ifs_appl_0x20
	ifs_weak_entry ifs_appl_0x21
	ifs_weak_entry ifs_appl_0x22
	ifs_weak_entry ifs_appl_0x23
	ifs_weak_entry ifs_appl_0x24
	ifs_weak_entry ifs_appl_0x25
	ifs_weak_entry ifs_appl_0x26
	ifs_weak_entry ifs_appl_0x27
	ifs_weak_entry ifs_appl_0x28
	ifs_weak_entry ifs_appl_0x29
	ifs_weak_entry ifs_appl_0x2a
	ifs_weak_entry ifs_appl_0x2b
	ifs_weak_entry ifs_appl_0x2c
	ifs_weak_entry ifs_appl_0x2d
	ifs_weak_entry ifs_appl_0x2e
	ifs_weak_entry ifs_appl_0x2f
	ifs_weak_entry ifs_appl_0x30
	ifs_weak_entry ifs_appl_0x31
	ifs_weak_entry ifs_appl_0x32
	ifs_weak_entry ifs_appl_0x33
	ifs_weak_entry ifs_appl_0x34
	ifs_weak_entry ifs_appl_0x35
	ifs_weak_entry ifs_appl_0x36
	ifs_weak_entry ifs_appl_0x37
	ifs_weak_entry ifs_appl_0x38
	ifs_weak_entry ifs_appl_0x39
	ifs_weak_entry ifs_appl_0x3a
	ifs_weak_entry ifs_appl_0x3b
	ifs_weak_entry ifs_appl_0x3c
	ifs_weak_entry ifs_sys_0x3d
	ifs_weak_entry ifs_sys_0x3e
	ifs_weak_entry ifs_sys_0x3f

