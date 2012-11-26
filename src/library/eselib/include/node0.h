/**
 * LIB: NODE1
 * 
 * This library contains macros useful for working on node1.
 * 
 **/
#ifndef __NODE0_H__
#define __NODE0_H__

#define NODE_ID							(0)

/* These leds are low active */
#define LEDS_PORT_ND0                                        PORTA
#define LEDS_DDR_ND0                                         DDRA
/* Sets pin to logic 1 in a byte */
#define LEDS_PIN_ND0(pin)                                   _BV(pin)

#endif
