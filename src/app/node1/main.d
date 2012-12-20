node1/main.d node1/main.o: node1/main.c platform.h
node1/main.o:
	avr-gcc -Wall -Wstrict-prototypes -Os -mmcu=atmega128 -ggdb -I../library/eselib/include -I../library/ttpa/include -I../library/commlib/include -I../library/scade/include -I./include -I../library/platform/include  -DPROTOCOL0  -DTIMER2  -DBARGRAPH  -DCLOCK  -DTHERMOMETER  -DCOOLER  -DHEATER -I./node1 -c -o node1/main.o node1/main.c
