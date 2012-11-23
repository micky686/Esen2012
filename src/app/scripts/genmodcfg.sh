#!/bin/bash
# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>
# empty config file

MODULE_CFG_FILE=${1}
MODULE_LIST=${2}

> ${MODULE_CFG_FILE}

# generate correct directory stack
for i in ${MODULE_LIST}; do
	echo  "# Module ${i}" >> ${MODULE_CFG_FILE}
	echo  "DIR := ${i}" >> ${MODULE_CFG_FILE}
	echo  "ifeq \"\$(shell [ -d \$(DIR) ] && echo 1)\" \"1\"" >> ${MODULE_CFG_FILE}
	echo  "include ${i}/makefile.mk" >> ${MODULE_CFG_FILE}
	echo  "" >> ${MODULE_CFG_FILE}
	echo  "else" >> ${MODULE_CFG_FILE}
	echo  "\$(warning \"Module \$(DIR) has been deleted. Force modcfg regeneration by removing \$(CONFIG_DIR) or simply call 'make clean all'\")" >> ${MODULE_CFG_FILE}
	echo  "endif" >> ${MODULE_CFG_FILE}
done
