node3/main.d node3/main.o: node3/main.c platform.h
node3/main.o:
	avr-gcc -Wall -Wstrict-prototypes -Os -mmcu=atmega128 -ggdb -I../library/eselib/include -I../library/ttpa/include -I../library/commlib/include -I../library/scade/include -I./include -I../library/platform/include  -DLEDMATRIX  -DSPI -I./node3 -c -o node3/main.o node3/main.c
