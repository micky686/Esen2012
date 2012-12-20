node2/main.d node2/main.o: node2/main.c platform.h
node2/main.o:
	avr-gcc -Wall -Wstrict-prototypes -Os -mmcu=atmega128 -ggdb -I../library/eselib/include -I../library/ttpa/include -I../library/commlib/include -I../library/scade/include -I./include -I../library/platform/include  -I./node2 -c -o node2/main.o node2/main.c
