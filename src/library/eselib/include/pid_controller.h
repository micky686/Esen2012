#ifndef __PID_CONTROLLER__
#define __PID_CONTROLLER__

//#define DOUBLE_AVAILABLE

#ifdef DOUBLE_AVAILABLE
	#define CTRL_WRK_TYPE		double
	#define CTRL_WRK_BIG_TYPE	double
#else
	#define CTRL_WRK_TYPE		int32_t
	#define CTRL_WRK_BIG_TYPE	int64_t
#endif

typedef struct
{
	CTRL_WRK_TYPE pro_factor;
	CTRL_WRK_TYPE int_factor;
	CTRL_WRK_TYPE der_factor;
	CTRL_WRK_BIG_TYPE int_term;
	CTRL_WRK_TYPE ub_int_term;
	CTRL_WRK_TYPE lb_int_term;
	CTRL_WRK_TYPE prev_error;
} ctrlpid_t;

CTRL_WRK_TYPE pidctrl_set_value(ctrlpid_t* pid, CTRL_WRK_TYPE err);
void pidctrl_init(ctrlpid_t* pid, CTRL_WRK_TYPE pro_factor, CTRL_WRK_TYPE int_factor, 
CTRL_WRK_TYPE der_factor, CTRL_WRK_TYPE ub_int_term, CTRL_WRK_TYPE lb_int_term);

#endif
