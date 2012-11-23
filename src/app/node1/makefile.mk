# Makefile fragment. Modulename is the directory name.
# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>

# mandatory prelude
include $(MSTDPRE)


# node modules must be added here
OBJ-M-$(MNAME)-y	+= main

# put the generic used library modules in here (need to be 
# defined in library/eselib/Makefile.eselib)
OBJ-ESEL-$(MNAME)-y	+= 

# put the module specific used library modules in here (need 
# to be defined in library/eselib/Makefile.eselib)
OBJ-ESEL-MDEP-$(MNAME)-y +=

# put the used scade models in here (need to be defined in 
# library/scade/Makefile.scade)
OBJ-SCDL-$(MNAME)-y	+=

# and finally the required system libraries (e.g. you would 
# put 'm' in there if you want to use the math library)
LIB-LD-$(MNAME)		:= m

# this module is actually an application and we need to hint 
# that. other (not implemented) possibilities could be archives,
# shared libraries, whatever
MOD-$(MNAME)-OUTPUT	:= $(MNAME).elf

# if this is a ttpa target we need some additional settings which 
# are ignored unless MODE in project.mk is set to ttpa.
#TTPA-$(MNAME)		:= -DMASTER $(ADD_NODECONF)
TTPA-$(MNAME)		:= -DSLAVE $(ADD_NODECONF) -DSW_UART_TXUSEPORT -DLN_NODE=0x11
# monitor code is only useful in node0
TTPA_MONITOR_ENABLED	:= 0

#mandatory postlude
include $(MSTDPOST)
