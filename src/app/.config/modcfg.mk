# Module ./node0
DIR := ./node0
ifeq "$(shell [ -d $(DIR) ] && echo 1)" "1"
include ./node0/makefile.mk

else
$(warning "Module $(DIR) has been deleted. Force modcfg regeneration by removing $(CONFIG_DIR) or simply call 'make clean all'")
endif
# Module ./node2
DIR := ./node2
ifeq "$(shell [ -d $(DIR) ] && echo 1)" "1"
include ./node2/makefile.mk

else
$(warning "Module $(DIR) has been deleted. Force modcfg regeneration by removing $(CONFIG_DIR) or simply call 'make clean all'")
endif
# Module ./node3
DIR := ./node3
ifeq "$(shell [ -d $(DIR) ] && echo 1)" "1"
include ./node3/makefile.mk

else
$(warning "Module $(DIR) has been deleted. Force modcfg regeneration by removing $(CONFIG_DIR) or simply call 'make clean all'")
endif
# Module ./doc
DIR := ./doc
ifeq "$(shell [ -d $(DIR) ] && echo 1)" "1"
include ./doc/makefile.mk

else
$(warning "Module $(DIR) has been deleted. Force modcfg regeneration by removing $(CONFIG_DIR) or simply call 'make clean all'")
endif
# Module ./node1
DIR := ./node1
ifeq "$(shell [ -d $(DIR) ] && echo 1)" "1"
include ./node1/makefile.mk

else
$(warning "Module $(DIR) has been deleted. Force modcfg regeneration by removing $(CONFIG_DIR) or simply call 'make clean all'")
endif
