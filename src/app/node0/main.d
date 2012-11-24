node0/main.d node0/main.o: node0/main.c ../library/eselib/include/global.h node0/main.h \
 ../library/eselib/include/node0.h ../library/commlib/include/protocoll.h \
 ../library/eselib/include/timer.h ../library/eselib/include/global.h \
 ../library/eselib/include/counter.h
node0/main.o:
	avr-gcc -Wall -Wstrict-prototypes -Os -mmcu=atmega128 -ggdb -I../library/eselib/include -I../library/ttpa/include -I../library/commlib/include -I../library/scade/include -I./include -I./node0 -c -o node0/main.o node0/main.c
