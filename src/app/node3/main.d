node3/main.d node3/main.o: node3/main.c node3/main.h ../library/eselib/include/global.h \
 ../library/eselib/include/node3.h
node3/main.o:
	avr-gcc -Wall -Wstrict-prototypes -Os -mmcu=atmega128 -ggdb -I../library/eselib/include -I../library/ttpa/include -I../library/scade/include -I./include -I./node3 -c -o node3/main.o node3/main.c
