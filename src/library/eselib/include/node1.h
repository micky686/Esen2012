/**
 * LIB: NODE1
 * 
 * This library contains macros useful for working on node1.
 * 
 * MatNr: 0326077
 * Date: 21.01.2012
 * @author Bernhard Froemel <bf@catechsoftworks.biz>
 **/
#ifndef __NODE1_H__
#define __NODE1_H__

#define NODE_ID             (1)


#define VENT_PIN            PB7 /* ventilator */

#define VENT_OFF()          do {DDRB |= 1<<VENT_PIN;  \
    PORTB &= ~(1<<VENT_PIN); } while (0)

#define VENT_ON()           do {DDRB |= 1<<VENT_PIN;  \
    PORTB |= (1<<VENT_PIN); } while (0)

#define HEATER_PIN          PB4 /* heater */

#define HEATER_OFF()        do {DDRB |= 1<<HEATER_PIN;  \
    PORTB |= (1<<HEATER_PIN); } while (0)

#define HEATER_ON()         do {DDRB |= 1<<HEATER_PIN;  \
    PORTB &= ~(1<<HEATER_PIN); } while (0)

#endif /*NODE1_H_*/
