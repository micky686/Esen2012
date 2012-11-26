#ifndef IFS_MEMSECT_H
#define IFS_MEMSECT_H
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
 * include/ifs_memsect.h
 *      Declarations of supportet IFS memory sections
 *
 * $Id: ifs_memsect.h,v 1.4 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/23 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *
 */
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
 
#define IFS_LOC_ifs_int 0x00
#define IFS_LOC_ifs_int_0 0x01
#define IFS_LOC_ifs_int_eep 0x02
#define IFS_LOC_ifs_int_flash 0x03
#define IFS_LOC_ifs_ext 0x04
#define IFS_LOC_ifs_ext_0 0x05
#define IFS_LOC_ifs_ext_eep 0x06
#define IFS_LOC_ifs_ext_flash 0x07
#define IFS_LOC_ifs_eep 0x0a
#define IFS_LOC_ifs_flash 0x0b
 
#define IFS_PERS_ifs_int 0x00
#define IFS_PERS_ifs_int_0 0x00
#define IFS_PERS_ifs_int_eep 0x01
#define IFS_PERS_ifs_int_flash 0x00
#define IFS_PERS_ifs_ext 0x00
#define IFS_PERS_ifs_ext_0 0x00
#define IFS_PERS_ifs_ext_eep 0x01
#define IFS_PERS_ifs_ext_flash 0x00
#define IFS_PERS_ifs_eep 0x02
#define IFS_PERS_ifs_flash 0x02

#define ifs_inb_mem(addr) *((uint8_t *)addr)

#define ifs_inb_eep(addr) eeprom_read_byte(addr)

#define ifs_inb_flash(addr) pgm_read_byte(addr)

#define ifs_outb_mem(addr, c) (*((uint8_t *)addr) = c)

#define ifs_outb_eep(addr, c)

#define ifs_outb_flash(addr, c)

//#define ifs_outb_eep(addr, c) do {c; addr;} while(0) // BUG ?

//#define ifs_outb_flash(addr, c) do {c; addr;} while(0)

#endif /* IFS_MEMSECT_H */
