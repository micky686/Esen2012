/*
 * platform.c
 *
 *  Created on: Dec 8, 2012
 *      Author: igor
 */

#include "platform.h"

void init_drivers(platform_t* platform){

#ifdef BARGRAPH
	bargraph_init();
	platform->drivers.set_bargraph = &set_bargraph;
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
void platform_init(config_platform_t conf, platform_t* platform){

	init_drivers(platform);


}




void run_platform(platform_t* platform){


}

int main(void) {

  platform_t platform;
  platform_init(platform_configuration, &platform);
  run_platform(&platform);

  while(TRUE);
  return 1;
}
