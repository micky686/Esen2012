# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>



# build final object list
ifeq "$(MODE)" "ttpa" # we need to do that byteconvert here as well
  OBJ-$(MNAME)	+= $(foreach m, $(OBJ-M-$(MNAME)-y), $(d)/$(m).io)  
else
  OBJ-$(MNAME)	+= $(foreach m, $(OBJ-M-$(MNAME)-y), $(d)/$(m).o)
endif
OBJ-$(MNAME)	+= $(foreach m, $(OBJ-ESEL-$(MNAME)-y), $(ESELIB_DIR)/src/$(m).o)
OBJ-$(MNAME)	+= $(foreach m, $(OBJ-ESEL-MDEP-$(MNAME)-y), $(d)/$(m).mdep.o)
SRC-MDEP-$(MNAME) += $(foreach m, $(OBJ-ESEL-MDEP-$(MNAME)-y), $(filter %.c %.S,$(wildcard $(ESELIB_DIR)/src/$(m).*)))
OBJ-$(MNAME)	+= $(foreach m, $(OBJ-SCDL-$(MNAME)-y), $(SCADE_DIR)/src/$(m).o)

# TTPA extension
ifeq "$(MODE)" "ttpa"
  ifneq (,$(findstring MASTER,$(TTPA-$(MNAME))))
    # add master objects
    _TMP_O := $(filter %.o,$(TTPA_MSTOBJ))
    OBJ-$(MNAME)+= $(foreach m, $(_TMP_O), $(d)/$(m:.o=.mdep.o))
# expand current content
    OBJ-$(MNAME):=$(OBJ-$(MNAME))
    _TMP_O := $(filter %.io,$(TTPA_MSTOBJ))
    OBJ-$(MNAME)+= $(foreach m, $(_TMP_O), $(d)/$(m:.io=.mdep.io))
# expand current content
    OBJ-$(MNAME):=$(OBJ-$(MNAME))
    SRC-MDEP-$(MNAME) += $(foreach m, $(TTPA_MSTOBJ), $(filter %.c %.S,$(wildcard $(TTPA_DIR)/ttpa/$(basename $(m)).*)))
  endif
  ifneq (,$(findstring SLAVE,$(TTPA-$(MNAME))))
    # add slave objects
    _TMP_O := $(filter %.o,$(TTPA_SLVOBJ))
    OBJ-$(MNAME)+= $(foreach m, $(_TMP_O), $(d)/$(m:.o=.mdep.o))
# expand current content
    OBJ-$(MNAME):=$(OBJ-$(MNAME)) 
   _TMP_O := $(filter %.io,$(TTPA_SLVOBJ))
    OBJ-$(MNAME)+= $(foreach m, $(_TMP_O), $(d)/$(m:.io=.mdep.io))
# expand current content
    OBJ-$(MNAME):=$(OBJ-$(MNAME))
    SRC-MDEP-$(MNAME) += $(foreach m, $(TTPA_SLVOBJ), $(filter %.c %.S,$(wildcard $(TTPA_DIR)/ttpa/$(basename $(m)).*)))
  endif
  ifeq "$(TTPA_MONITOR_ENABLED)" "1"
    # add optional(?) monitor objects
    _TMP_O := $(filter %.o,$(TTPA_MONOBJ))
    OBJ-$(MNAME)+= $(foreach m, $(_TMP_O), $(d)/$(m:.o=.mdep.o))
# expand current content
    OBJ-$(MNAME):=$(OBJ-$(MNAME))
    _TMP_O := $(filter %.io,$(TTPA_MONOBJ))
    OBJ-$(MNAME)+= $(foreach m, $(_TMP_O), $(d)/$(m:.io=.mdep.io))
# expand current content
    OBJ-$(MNAME):=$(OBJ-$(MNAME))
    SRC-MDEP-$(MNAME) += $(foreach m, $(TTPA_MONOBJ), $(filter %.c %.S,$(wildcard $(TTPA_DIR)/ttpa/$(basename $(m)).*)))
  endif
  ifeq "$(TTPA_MONITOR_ENABLED)" "2"
    # add optional(?) signal objects
    _TMP_O := $(filter %.o,$(TTPA_SIGOBJ))
    OBJ-$(MNAME)+= $(foreach m, $(_TMP_O), $(d)/$(m:.o=.mdep.o))
# expand current content
    OBJ-$(MNAME):=$(OBJ-$(MNAME))
    _TMP_O := $(filter %.io,$(TTPA_SIGOBJ))
    OBJ-$(MNAME)+= $(foreach m, $(_TMP_O), $(d)/$(m:.io=.mdep.io))
# expand current content
    OBJ-$(MNAME):=$(OBJ-$(MNAME))
    SRC-MDEP-$(MNAME) += $(foreach m, $(TTPA_SIGOBJ), $(filter %.c %.S,$(wildcard $(TTPA_DIR)/ttpa/$(basename $(m)).*)))
  endif
  # add TTPA macros to CFLAGS
  CFLAGS-$(MNAME) += $(TTPA-$(MNAME))
endif

# keep track globally of all objects
OBJ		+= $(OBJ-$(MNAME))

# necessary to evaluate the whole expression. TODO: find a better way
OBJ-$(MNAME)	:= $(OBJ-$(MNAME))
SRC-MDEP-$(MNAME) := $(SRC-MDEP-$(MNAME))

#complete library includes
LIB-LD-$(MNAME)	:= $(addprefix -l,$(LIB-LD-$(MNAME)))

ifneq "$(MOD-$(MNAME)-OUTPUT)" ""
MOD-$(MNAME)-OUTPUT	:= $(d)/$(MOD-$(MNAME)-OUTPUT)
# build final output list
OUTPUT			+= $(MOD-$(MNAME)-OUTPUT)
endif

# add individual dependencies
$(MOD-$(MNAME)-OUTPUT): $(OBJ-$(MNAME))


# pop directory
-include $(DEPS_$(d))
d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))
