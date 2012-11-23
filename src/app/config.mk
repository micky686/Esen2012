# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>

DEBUGPORT	= 6000
#old ESE board:
#INTERFACE	= /dev/ttyUSB2
#new ESE board:
PRGMIF		= /dev/ttyACM0

MCU			= atmega128
FORMAT		= ihex


CFLAGS		:= -Wall -Wstrict-prototypes -Os -mmcu=${MCU} -ggdb


ASFLAGS		= -mmcu=${MCU} -Wa,-mmcu=${MCU} -ggdb
LDFLAGS		= -ggdb -mmcu=${MCU}



CC		= avr-gcc
LD		= avr-ld
AS		= avr-as
OBJCOPY		= avr-objcopy
SIZE		= avr-size

CHMOD		= chmod
SED		= sed

#old ESE board:
#PROGR		= avarice
#MUXR		= JTAGZeusProg
#new ESE board:
PROGR		= jtagprog

PDFLTX		= pdflatex

DOPT		= -MM

# TTPA
EEPFLAGS	= -j .eeprom --change-section-lma .eeprom-0x810000 \
                  --set-section-flags=.eeprom="alloc,load" \
                  -j .ifs_eep --change-section-lma .ifs_eep-0x810000 \
                  --set-section-flags=.ifs_eep="alloc,load" \
                  -j .ifs_int_eep --change-section-lma .ifs_int_eep-0x810000 \
                  --set-section-flags=.ifs_int_eep="alloc,load" \
                  -j .ifs_ext_eep --change-section-lma .ifs_ext_eep-0x810000 \
                  --set-section-flags=.ifs_ext_eep="alloc,load"
HEXFLAGS	= -j .text -j .data -j .ifs_int_flash -j .ifs_ext_flash

ADD_NODECONF	= -DBAUDRATE=9600 -DNODECONF_MEGA128_V4_0 -DSW_UART -DUSE_SCHEDULE

# TDIR is defined in top level Makefile
MSTDPRE		= $(TDIR)/scripts/modpre.mk
MSTDPOST	= $(TDIR)/scripts/modpost.mk

SCR_GENMOD	= $(TDIR)/scripts/genmodcfg.sh
SCR_DEPEND	= $(TDIR)/scripts/depend.sh
SCR_MDEPEND	= $(TDIR)/scripts/mdepend.sh
SCR_FILTER	= $(TDIR)/scripts/filter.sh
SCR_SPELLCHECK	= $(TDIR)/scripts/spellcheck.sh

CONFIG_DIR	= .config
BUILD_DIR	= build
LIB_DIR		= ../library
ESELIB_DIR	= $(LIB_DIR)/eselib
TTPA_DIR	= $(LIB_DIR)/ttpa
TTPALD_DIR	= $(TTPA_DIR)/ldscripts
SCADE_DIR	= $(LIB_DIR)/scade
LDD_DIR		= $(TTPA_DIR)/ldscripts/
IFSCONV_SCR	= $(TTPA_DIR)/scripts/conv_byteorder.pl

# add include directories of the various libraries
CFLAGS		+= -I$(ESELIB_DIR)/include -I$(TTPA_DIR)/include -I$(SCADE_DIR)/include -I./include
#use special linker script if this is a ttpa application
ifeq "$(MODE)" "ttpa"
  LDFLAGS	+= -Xlinker -T -Xlinker ${TTPALD_DIR}/${MCU}.x
endif

# add SCADE_DIR and ESELIB_DIR to cleanpath
CLEANPATH	+= $(ESELIB_DIR) $(SCADE_DIR)

# VPATH settings
VPATH_HEADER	:= ./include:$(ESELIB_DIR)/include/:$(TTPA_DIR)/include/:$(SCADE_DIR)/include/
VPATH_CFILES	:= $(ESELIB_DIR)/src/:$(TTPA_DIR)/ttpa/:$(SCADE_DIR)/src/
VPATH_SFILES	:= $(ESELIB_DIR)/src/:$(TTPA_DIR)/ttpa/:$(SCADE_DIR)/src/

# include other configuration
include $(LIB_DIR)/Makefile.ttpa
include $(LIB_DIR)/Makefile.scade
