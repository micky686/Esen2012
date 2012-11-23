../library/eselib/src/uart.d ../library/eselib/src/uart.o: ../library/eselib/src/uart.c ../library/eselib/include/global.h \
 ../library/eselib/include/uart.h
../library/eselib/src/uart.o:
	avr-gcc -Wall -Wstrict-prototypes -Os -mmcu=atmega128 -ggdb -I../library/eselib/include -I../library/ttpa/include -I../library/scade/include -I./include  -c -o ../library/eselib/src/uart.o ../library/eselib/src/uart.c
