# ESE project template
# Together with Makefile.ESELU and config.mk this customizes the general 
# Makefile build-environment.
#
# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>

PROJNAME	= blinking_leds

# language used for spellchecking
LANG		= de

# default or ttpa (ttpa requires special libraries and a different link script)
MODE		= default

# required for the flash_all target: define all participating nodes here
# note that this also specifies the flashing order: e.g. 
# FLASH_NODES = 0 1 
# would mean first 1 then 0 (-> backwards) 
FLASH_NODES	= 3 2 1 0


# target dependent checks

ifneq "$(MODE)" "default"
  ifneq "$(MODE)" "ttpa"
    $(error "Unknown application mode (check project.mk, MODE)")
  endif
endif
