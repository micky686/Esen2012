/*
 * ldscripts/atmega128.x
 *      Linker script for Atmel ATmega128
 *
 * $Id: atmega128.x,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   original version taken from avr-libc V1.0.2, written by 
 *     Marek Michalkiewicz <marekm@amelek.gda.pl>
 *
 *   2004/02/26 (0.1): Modification for TTP/A protocol (special sections for
 *     IFS files, tables, EEPROM handling; Christian Trödhandl
 *     <chtr@vmars.tuwien.ac.at>
 *   2005/08/05 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     Added symbols sched_init_ttab, sched_init_ttab_end, sched_bg_ttab,
 *     sched_bg_ttab_end
 *
 */

OUTPUT_FORMAT("elf32-avr","elf32-avr","elf32-avr")
OUTPUT_ARCH(avr:5)
MEMORY
{
  text   (rx)   : ORIGIN = 0, LENGTH = 128K
  data   (rw!x) : ORIGIN = 0x800060, LENGTH = 0xffa0
  eeprom (rw!x) : ORIGIN = 0x810000, LENGTH = 64K
}
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  .hash          : { *(.hash)		}
  .dynsym        : { *(.dynsym)		}
  .dynstr        : { *(.dynstr)		}
  .gnu.version   : { *(.gnu.version)	}
  .gnu.version_d   : { *(.gnu.version_d)	}
  .gnu.version_r   : { *(.gnu.version_r)	}
  .rel.init      : { *(.rel.init)		}
  .rela.init     : { *(.rela.init)	}
  .rel.text      :
    {
      *(.rel.text)
      *(.rel.text.*)
      *(.rel.gnu.linkonce.t*)
    }
  .rela.text     :
    {
      *(.rela.text)
      *(.rela.text.*)
      *(.rela.gnu.linkonce.t*)
    }
  .rel.fini      : { *(.rel.fini)		}
  .rela.fini     : { *(.rela.fini)	}
  .rel.rodata    :
    {
      *(.rel.rodata)
      *(.rel.rodata.*)
      *(.rel.gnu.linkonce.r*)
    }
  .rela.rodata   :
    {
      *(.rela.rodata)
      *(.rela.rodata.*)
      *(.rela.gnu.linkonce.r*)
    }
  .rel.data      :
    {
      *(.rel.data)
      *(.rel.data.*)
      *(.rel.gnu.linkonce.d*)
    }
  .rela.data     :
    {
      *(.rela.data)
      *(.rela.data.*)
      *(.rela.gnu.linkonce.d*)
    }
  .rel.ctors     : { *(.rel.ctors)	}
  .rela.ctors    : { *(.rela.ctors)	}
  .rel.dtors     : { *(.rel.dtors)	}
  .rela.dtors    : { *(.rela.dtors)	}
  .rel.got       : { *(.rel.got)		}
  .rela.got      : { *(.rela.got)		}
  .rel.bss       : { *(.rel.bss)		}
  .rela.bss      : { *(.rela.bss)		}
  .rel.plt       : { *(.rel.plt)		}
  .rela.plt      : { *(.rela.plt)		}
  /* Internal text space or external memory */
  .text :
  {
    *(.vectors)
     __ctors_start = . ;
     *(.ctors)
     __ctors_end = . ;
     __dtors_start = . ;
     *(.dtors)
     __dtors_end = . ;
    *(.progmem.gcc*)
    . = ALIGN(4);
    *(.ttpa_ifstab)
     ifs_hrec_start = . ;
    *(.ttpa_ifshrec)
     ifs_hrec_end = . ;
     PROVIDE (sched_init_ttab = .) ;
    *(.sched_init.0)
    *(.sched_init.1)
    *(.sched_init.2)
    *(.sched_init.3)
    *(.sched_init.4)
    *(.sched_init.5)
    *(.sched_init.6)
    *(.sched_init.7)
    *(.sched_init.8)
    *(.sched_init.9)
    *(.sched_init.10)
    *(.sched_init.11)
    *(.sched_init.12)
    *(.sched_init.13)
    *(.sched_init.14)
    *(.sched_init.15)
     PROVIDE (sched_init_ttab_end = .) ;
     PROVIDE (sched_bg_ttab = .) ;
    *(.sched_bg)
     PROVIDE (sched_bg_ttab_end = .) ;
     . = ALIGN(4);
    *(.ifs_flash)
    *(.progmem*)
    . = ALIGN(2);
    *(.init0)  /* Start here after reset.  */
    *(.init1)
    *(.init2)  /* Clear __zero_reg__, set up stack pointer.  */
    *(.init3)
    *(.init4)  /* Initialize data and BSS.  */
    *(.init5)
    *(.init6)  /* C++ constructors.  */
    *(.init7)
    *(.init8)
    *(.init9)  /* Call main().  */
    *(.text)
    . = ALIGN(2);
    *(.text.*)
    . = ALIGN(2);
    *(.fini9)  /* _exit() starts here.  */
    *(.fini8)
    *(.fini7)
    *(.fini6)  /* C++ destructors.  */
    *(.fini5)
    *(.fini4)
    *(.fini3)
    *(.fini2)
    *(.fini1)
    *(.fini0)  /* Infinite loop after program termination.  */
     _etext = . ;
  }  > text
  .data	  : AT (ADDR (.text) + SIZEOF (.text))
  {
     PROVIDE (__data_start = .) ;
    *(.data)
    *(.gnu.linkonce.d*)
    . = ALIGN(2);
     _edata = . ;
     PROVIDE (__data_end = .) ;
  }  > data
  .bss : AT (SIZEOF(.data) + ADDR(.data)) 
  {
     PROVIDE (__bss_start = .) ;
    *(.bss)
    *(COMMON)
     PROVIDE (__bss_end = .) ;
  }  > data /*** The linker tends to load it in text area ***/
   __data_load_start = LOADADDR(.data);
   __data_load_end = __data_load_start + SIZEOF(.data);
  /* Global data not cleared after reset.  */
  .noinit  SIZEOF(.bss) + ADDR(.bss) :
  {
     PROVIDE (__noinit_start = .) ;
    *(.noinit*)
     PROVIDE (__noinit_end = .) ;
     _end = . ;
  }   > data 
  .ifs_int SIZEOF(.noinit) + ADDR(.noinit) :
  {
     PROVIDE (__ifs_int_start = .) ;
     *(.ifs_int)
     PROVIDE (__ifs_int_end = .) ;
  }   > data 
  .ifs_int_0 SIZEOF(.ifs_int) + ADDR(.ifs_int) :
  {
     PROVIDE (__ifs_int_0_start = .) ;
     *(.ifs_int_0)
     PROVIDE (__ifs_int_0_end = .) ;
  }  > data 
  .eeprom  :
  {
    *(.eeprom*)
     __eeprom_end = . ;
  }  > eeprom
  .ifs_eep SIZEOF(.eeprom) + ADDR(.eeprom) :
  {
    PROVIDE (__ifs_eep_start = .) ;
    *(.ifs_eep)
    PROVIDE (__ifs_eep_end = .) ;
  } > eeprom
  .ifs_int_eep SIZEOF(.ifs_int_0) + ADDR(.ifs_int_0) : /*** AT (ADDR (.ifs_eep) + SIZEOF (.ifs_eep)) ***/
  {
     PROVIDE (__ifs_int_eep_start = .) ;
     *(.ifs_int_eep)
     PROVIDE (__ifs_int_eep_end = .) ;
  }  > data AT> eeprom 
  __ifs_int_eep_loadstart = LOADADDR(.ifs_int_eep);
  __ifs_int_eep_loadend = __ifs_int_eep_loadstart + SIZEOF(.ifs_int_eep);
  .ifs_int_flash SIZEOF(.ifs_int_eep) + ADDR(.ifs_int_eep) : AT (ADDR (.text) + SIZEOF (.text) + SIZEOF (.data))
  {
     PROVIDE (__ifs_int_flash_start = .) ;
     *(.ifs_int_flash)
     PROVIDE (__ifs_int_flash_end = .) ;
  }  > data
  __ifs_int_flash_loadstart = LOADADDR(.ifs_int_flash);
  __ifs_int_flash_loadend = __ifs_int_flash_loadstart + SIZEOF(.ifs_int_flash);

  /*** The following lines were added in order to make the linker ***
   *** to place these sections (sched_queue and IO_SEC) into data region
   *** and not to the text region ***/
  .sched_queue : AT (SIZEOF (.ifs_int_flash) + ADDR (.ifs_int_flash))
  {
	*(.sched_queue);
  } 

  .IO_SEC : AT(SIZEOF(.sched_queue) + ADDR(.sched_queue))
  {
     *(.IO_SEC);
     PROVIDE (__heap_start = .) ;
  } 
  
  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) *(.gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
}
