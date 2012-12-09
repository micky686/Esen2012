/*
 * platform.c
 *
 *  Created on: Dec 8, 2012
 *      Author: igor
 */

#include "platform.h"

volatile platform_t platform;

void init_drivers(void){

#ifdef BARGRAPH
	bargraph_init();
	platform.drivers.set_bargraph = &set_bargraph;
#endif

#ifdef PROTOCOLL

#endif

#ifdef TIMER2

#endif

#ifdef CLOCK

#endif


#ifdef COOLER

#endif


#ifdef HEATER

#endif


#ifdef DISPLAY

#endif

#ifdef THERMOMETER

#endif


#ifdef PUSHBUTTON

#endif

}


/*
 * Initialize the platform with the provided configuration and
 * setup all requested drivers.
 */
void platform_init(void){

	init_drivers();


}




void run_platform(void){


}

int main(void) {


  platform_init();
  run_platform();

  while(TRUE);
  return 1;
}
