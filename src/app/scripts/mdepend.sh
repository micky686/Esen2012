#!/bin/sh
# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>

STEM="$1"
SUFFIXO="$2"
SUFFIXD="$3"
shift 3
${CC} -MM -MG "$@" |
${SED} -e "s@^\(.*\)\:@$STEM.$SUFFIXO $STEM.$SUFFIXD:@"
