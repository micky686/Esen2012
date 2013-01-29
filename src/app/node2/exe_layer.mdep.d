node2/exe_layer.mdep.o node2/exe_layer.mdep.d: ../library/platform/src/exe_layer.c \
 ../library/platform/include/exe_layer.h \
 ../library/platform/include/platform.h \
 ../library/eselib/include/global.h ../library/eselib/include/bargraph.h \
 ../library/eselib/include/thermometer.h \
 ../library/eselib/include/cooler.h ../library/eselib/include/heater.h \
 ../library/eselib/include/DISPLAY.h \
 ../library/eselib/include/protocol0.h ../library/eselib/include/timer2.h
node2/exe_layer.mdep.o:
	avr-gcc -Wall -Wstrict-prototypes -Os -mmcu=atmega128 -ggdb -I../library/eselib/include -I../library/ttpa/include -I../library/commlib/include -I../library/scade/include -I./include -I../library/platform/include  -I./node2  -c -o node2/exe_layer.mdep.o ../library/platform/src/exe_layer.c
