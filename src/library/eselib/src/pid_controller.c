/**======================================================================/
 * Project    : Exercise 2 
 * Application: pid_regler.c 
 * Note       : 
 * Revision   : 1.5
 * Author     : Bernhard Frömel, 0326077, e0326077@student.tuwien.ac.at  
 * 			  : Ruhul Alam, 0125788 , e0125788@student.tuwien.ac.at
 *            : Gaisbauer David, 0225034 , e0125788@student.tuwien.ac.at
 * 
 * Date       : 2006/10/28 
 *              TU Vienna
 *======================================================================*/
#include <avr/io.h>
#include "pid_controller.h"

#define ERR_P_FACTOR		100
#define ERR_I_FACTOR		1000
#define ERR_D_FACTOR		100
#define P_CUTOFF			15


void pidctrl_init(ctrlpid_t* pid, CTRL_WRK_TYPE pro_factor, CTRL_WRK_TYPE int_factor, 
CTRL_WRK_TYPE der_factor, CTRL_WRK_TYPE ub_int_term, CTRL_WRK_TYPE lb_int_term)
{
	pid->pro_factor = pro_factor;
	pid->int_factor = int_factor;
	pid->der_factor = der_factor;
	pid->ub_int_term = ub_int_term;
	pid->lb_int_term = lb_int_term;
}

CTRL_WRK_TYPE pidctrl_set_value(ctrlpid_t* pid, CTRL_WRK_TYPE err)
{
	CTRL_WRK_TYPE pro_gain = 0;
	CTRL_WRK_TYPE int_gain = 0;
	CTRL_WRK_TYPE der_gain = 0;
	
	if( (err < -(P_CUTOFF)) || (err > P_CUTOFF) )
	{
		pro_gain = (pid->pro_factor *  err)/ERR_P_FACTOR;
	}
	
	/*
	 * Integral term
	 */
	pid->int_term += err;
	
	if(pid->int_term > pid->ub_int_term)
	{
		pid-> int_term = pid->ub_int_term;
	}
	else if(pid->int_term < pid->lb_int_term)
	{
		pid->int_term = pid->lb_int_term;
	}
	
	int_gain = (pid->int_factor * pid->int_term)/ERR_I_FACTOR;

	/*
	 * derivative term
	 */
	der_gain = (pid->der_factor * ( err - pid->prev_error))/ERR_D_FACTOR;
	
	pid->prev_error = err;
	
	return pro_gain + int_gain + der_gain;
}
