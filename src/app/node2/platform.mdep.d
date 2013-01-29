node2/platform.mdep.o node2/platform.mdep.d: ../library/platform/src/platform.c \
 ../library/platform/include/platform.h \
 ../library/eselib/include/global.h ../library/eselib/include/bargraph.h \
 ../library/eselib/include/thermometer.h \
 ../library/eselib/include/cooler.h ../library/eselib/include/heater.h \
 ../library/eselib/include/DISPLAY.h \
 ../library/eselib/include/protocol0.h ../library/eselib/include/timer2.h \
 ../library/platform/include/hw_layer.h \
 ../library/platform/include/scheduler.h \
 ../library/platform/include/platform.h \
 ../library/platform/include/exe_layer.h
node2/platform.mdep.o:
	avr-gcc -Wall -Wstrict-prototypes -Os -mmcu=atmega128 -ggdb -I../library/eselib/include -I../library/ttpa/include -I../library/commlib/include -I../library/scade/include -I./include -I../library/platform/include  -I./node2  -c -o node2/platform.mdep.o ../library/platform/src/platform.c
