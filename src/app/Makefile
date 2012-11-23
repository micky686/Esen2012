# Generic Makefile V 0.2
# This generic Makefile manages program modules and can produce elf files.
# 
# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>


# project specific settings
include project.mk

#"trick" to get a space variable
NULLSTRING :=
SPACE := $(NULLSTRING) # EOF


TDIR := $(shell pwd)
# properly escape path
TDIR := $(subst $(SPACE),\ ,$(TDIR))
.DEFAULT_GOAL	:= all

# cleanpath and cleanfile
CLEANPATH := ./
CLEANFILE :=


include config.mk

# either we define the modules here or search for them
EXISTMOD = $(shell find . -name makefile.mk)
MODULES := $(patsubst %/makefile.mk, %, $(EXISTMOD))
# Makefile extensions can contribute to this
PHONY	:= all clean cleandep distclean

_dummy := $(shell [ -d $(CONFIG_DIR) ] || (mkdir $(CONFIG_DIR); ${SCR_GENMOD} $(CONFIG_DIR)/modcfg.mk "$(MODULES)"))
# clean modrules.mk which contains custom rules for single modules

vpath %.h $(VPATH_HEADER)
vpath %.c $(VPATH_CFILES)
vpath %.S $(VPATH_SFILES)


# each module will contribute to the following variables
# look for include files TODO: make that module specific!
# CFLAGS += -Iinclude $(patsubst %,-I%, $(MODULES))
 
# libraries
LIBS :=
# object files
OBJ :=
# output files
OUTPUT :=

# include description for each module
include $(CONFIG_DIR)/modcfg.mk

# determine the source files
EXISTC = $(foreach mod, $(MODULES), $(shell find $(mod) -maxdepth 1 -name "*.c"))
EXISTS = $(foreach mod, $(MODULES), $(shell find $(mod) -maxdepth 1 -name "*.S"))
POTC   = $(OBJ:.o=.c)
POTS   = $(OBJ:.o=.S)
CFILES = $(filter $(EXISTC), $(POTC))
SFILES = $(filter $(EXISTS), $(POTS))

BOTH   = $(filter $(CFILES:.c=), $(SFILES:.S=))



ifneq "$(strip $(BOTH))" ""
     $(error Both $(addsuffix .c, $(BOTH)) and $(addsuffix .S, $(BOTH)) exist.)
endif

# remove duplicates from OBJ and OUTPUT
OBJ	:= $(sort $(OBJ))
OUTPUT	:= $(sort $(OUTPUT))


all: $(OUTPUT)

# dependencies are individually added (see modpost.mk)
%.elf:
	$(CC) -Wl,-Map=$*.map $(LDFLAGS) -o $@ $(OBJ-$(@F:.elf=)) $(LIB-LD-$(@F:.elf=))


# calculate C include dependencies + append custom compile commands: 
# - module specific CFLAGS (-> correct include directory)

%.d: %.c
	rm -f $@
	CC=$(CC) SED=$(SED) ${SCR_DEPEND} `dirname $*.c` o d $(CFLAGS) $*.c > $@
	echo "$(@:.d=.o):" >> $@
	echo "	$(CC) $(CFLAGS) $(CFLAGS-$(patsubst _%,%, $(subst ..,,$(subst ./,,$(subst /,_,$(@D)))))) -c -o $(@:.d=.o) $(@:.d=.c)" >> $@

%.h:
ifeq "$(filter distclean cleandep,$(MAKECMDGOALS))" ""
	$(error Header file "$@" is missing!)
else
	$(warning Header file "$@" is missing!)
endif

# include the C include dependencies (made weak to avoid errors)
# if the include files don't exist (as they do at the very first call or a 
# dependency change) then make will automatically try to create this 
# missing dependency file(s)
-include $(patsubst %.o,%.d,$(filter %.o,$(OBJ)))
	
clean:
	rm -f $(OBJ) $(OUTPUT) $(patsubst %.elf,%.map,$(filter %.elf, $(OUTPUT)))
	rm -f -R .config
	rm -f -R $(BUILD_DIR)
	rm -f -R $(CLEANFILE)

cleandep: clean
	rm -f $(patsubst %.o,%.d,$(filter %.o, $(OBJ)))
	rm -f $(patsubst %.io,%.id,$(filter %.io, $(OBJ)))
	

# radically remove anything that remotely looks like a backup, a d file, 
# an object file or an elf file.
distclean: cleandep
	for i in `find $(CLEANPATH) -name "*.d"`; do rm $$i; done
	for i in `find $(CLEANPATH) -name "*.o"`; do rm $$i; done
	for i in `find $(CLEANPATH) -name "*.elf"`; do rm $$i; done
	for i in `find $(CLEANPATH) -name "*~"`; do rm $$i; done
	for i in `find $(CLEANPATH) -name "*.bak"`; do rm $$i; done

## SECONDEXPANSION targets ##

# I call that at least strange behavior: if I do it that way it works
# if I replace _mdep_bug with it's content in the prerequisite list
# (properly escaped) it doesn't work.
# note: .S overrides .c files here, but we check that one module has 
# either a .S or a .c but not both.
_mdep_bug = $(filter %$(subst .mdep.d,,$(@F)).S,$(SRC-MDEP-$(patsubst _%,%,$(subst ..,,$(subst ./,,$(subst /,_,$(@D))))))) \
            $(filter %$(subst .mdep.d,,$(@F)).c,$(SRC-MDEP-$(patsubst _%,%,$(subst ..,,$(subst ./,,$(subst /,_,$(@D)))))))
.SECONDEXPANSION:
%.mdep.d: $$(_mdep_bug) 
	rm -f $@
	CC=$(CC) SED=$(SED) ${SCR_MDEPEND} $(@:.d=) o d $(CFLAGS) $(CFLAGS-$(patsubst _%,%, $(subst ..,,$(subst ./,,$(subst /,_,$(@D)))))) $< > $@
	echo "$(@:.d=.o):" >> $@
	echo "	$(CC) $(CFLAGS) $(CFLAGS-$(patsubst _%,%, $(subst ..,,$(subst ./,,$(subst /,_,$(@D)))))) $(CFLAGS-ESEL-MDEP-$(patsubst _%,%, $(subst ..,,$(subst ./,,$(subst /,_,$(@D)))))) -c -o $(@:.d=.o) $<" >> $@
	

# include target specific extensions (like special install targets, aso.)
include Makefile.ESELU

# list of all non-file targets
.PHONY: $(PHONY)




.SUFFIXES:
