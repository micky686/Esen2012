/** 
 * blinking_leds
 *
 * (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>
 **/
#ifndef __MAIN_H__
#define __MAIN_H__

#define NODE 1

#include "platform.h"

config_platform_t conf;
char drivers[][50] = {"driver1", "driver2"};
conf.drivers = drivers;
conf.agent0_active=1;
conf.agent0_prio=3;
conf.agent0_code="code";

conf_p = conf;

#endif
