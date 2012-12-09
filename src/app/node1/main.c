/** 
 * blinking_leds (N1-3)
 *
 * (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>
 **/
#include "platform.h"

void hallo(void){

ACTIVATE_AGENT(0);
SET_AGENT_PRIO(0, 3);
SET_AGENT_CODE(0,00000100000000000000000000001);

}
