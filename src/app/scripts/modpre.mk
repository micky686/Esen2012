# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>

sp			:= $(sp).x
dirstack_$(sp)		:= $(d)
d			:= $(DIR)

# initialize local variables
TTPA_MONITOR_ENABLED	:= 0

# replace / with _ and . with nothing
MNAME		:= $(subst .,,$(subst /,_,$(d)))
# get rid of the first _
MNAME		:= $(patsubst _%,%, $(MNAME))

# module directory must also be in include path - but module specific
CFLAGS-$(MNAME) := -I$(d)
# empty MDEP CFLAGS
CFLAGS-ESEL-MDEP-$(MNAME) :=
