/** 
 * blinking_leds
 *
 **/
 
#ifndef __MAIN_H__
#define __MAIN_H__

#define NODE 0
#include "platform.h"

char* drivers[DRV_LEN] = {"bargraph", "driver2"};

 platform_configuration = {
		 .drivers = drivers;
		 .agent0_active=1;
		 .agent0_prio=3;
		 .agent0_code="code";
 }


#endif
