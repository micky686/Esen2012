#!/bin/sh
# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>

DIR="$1"
SUFFIXO="$2"
SUFFIXD="$3"
shift 3
case "$DIR" in
"" | ".")
${CC} -MM -MG "$@" |
${SED} -e "s@^\(.*\)\.o:@\1.$SUFFIXD \1.$SUFFIXO:@"
;;
*)
${CC} -MM -MG "$@" |
${SED} -e "s@^\(.*\)\.o:@$DIR/\1.$SUFFIXD $DIR/\1.$SUFFIXO:@"
;;
esac
