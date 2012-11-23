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
 * ttpa/ifs_init.c
 *      IFS initialisation code
 *
 * $Id: ifs_init.c,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     TODO: - change to file specific initialization when eepromfs is 
 *             implemented
 *   2004/06/14 (0.3): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added ADD_INITTASK
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed USE_EXT_RAM
 *   2005/10/28 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - fixed sign promotion bug in ifs_init
 *
 */
#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "node_config.h"
#include "ttpa.h"
#include "ifs.h"
#include "ifs_init.h"
#include "schedule.h"
#include "longptr.h"

extern ifs_inode_t ifs_hrec_start[];

extern ifs_inode_t ifs_hrec_end[];

extern uint8_t __ifs_int_start[];

extern uint8_t __ifs_int_end[];

extern uint8_t __ifs_int_0_start[];

extern uint8_t __ifs_int_0_end[];

extern uint8_t __ifs_int_eep_start[];

extern uint8_t __ifs_int_eep_end[];

extern uint8_t __ifs_int_eep_loadstart[];

extern uint8_t __ifs_int_eep_loadend[];

extern uint8_t __ifs_int_flash_start[];

extern uint8_t __ifs_int_flash_end[];

extern uint8_t __ifs_int_flash_loadstart[];

extern uint8_t __ifs_int_flash_loadend[];

#if defined (USE_EXT_RAM)
extern uint8_t __ifs_ext_start[];

extern uint8_t __ifs_ext_end[];

extern uint8_t __ifs_ext_0_start[];

extern uint8_t __ifs_ext_0_end[];

extern uint8_t __ifs_ext_eep_start[];

extern uint8_t __ifs_ext_eep_end[];

extern uint8_t __ifs_ext_eep_loadstart[];

extern uint8_t __ifs_ext_eep_loadend[];

extern uint8_t __ifs_ext_flash_start[];

extern uint8_t __ifs_ext_flash_end[];

extern uint8_t __ifs_ext_flash_loadstart[];

extern uint8_t __ifs_ext_flash_loadend[];
#endif /* USE_EXT_RAM */

extern uint8_t __ifs_eep_start[];

extern uint8_t __ifs_eep_end[];

/*
 * int ifs_init(void): Initialize IFS files
 */
int ifs_init(void)
{
	uint8_t *src, *dest;
#if FLASHEND >0xffff
	uint32_t flash_src;
#define flash_read_byte(addr) pgm_read_byte_far(addr)
#else
	uint16_t flash_src;
#define flash_read_byte(addr) pgm_read_byte_near(addr)
#endif /* FLASHEND */

	// initialize .ifs_int_0
	for(dest = __ifs_int_0_start; dest < __ifs_int_0_end; dest++) {
		*dest = 0x00;
	}
	// poll until eeprom is ready
	while(!eeprom_is_ready()) ;
	src =__ifs_int_eep_loadstart;
	for(dest = __ifs_int_eep_start; dest < __ifs_int_eep_end; dest++) {
		*dest = eeprom_read_byte(src++);
	}
#if FLASHEND >0xffff
	flash_src = ptr_to_ui32(__ifs_int_flash_loadstart);
#else
	flash_src = __ifs_int_flash_loadstart;
#endif /* FLASHEND */
	for(dest = __ifs_int_flash_start; dest < __ifs_int_flash_end; dest++) {
		*dest = flash_read_byte(flash_src++);
	}
#if defined (USE_EXT_RAM)
	// initialize .ifs_ext_0
	for(dest = __ifs_ext_0_start; dest < __ifs_ext_0_end; dest++) {
		*dest = 0x00;
	}
	// poll until eeprom is ready
	while(!eeprom_is_ready()) ;
	src =__ifs_ext_eep_loadstart;
	for(dest = __ifs_ext_eep_start; dest < __ifs_ext_eep_end; dest++) {
		*dest = eeprom_read_byte(src++);
	}
#if FLASHEND >0xffff
	flash_src = ptr_to_ui32(__ifs_ext_flash_loadstart);
#else
	flash_src = __ifs_ext_flash_loadstart;
#endif /* FLASHEND */
	for(dest = __ifs_ext_flash_start; dest < __ifs_ext_flash_end; dest++) {
		*dest = flash_read_byte(flash_src++);
	}
#endif /* USE_EXT_RAM */
	return 0;
}

ADD_INITTASK(task_ifs_init, ifs_init, 4, (1<<TTPA_STATE_UNSYNC));

/*
 * void ifs_init_file(ifs_inodeptr_t inode): IFS file specific initialization
 */
void ifs_init_file(ifs_inodeptr_t inode)
{
}
