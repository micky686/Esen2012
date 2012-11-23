#include <inttypes.h>

#include "ms_request.h"
#include "ms_master.h"
#include "ms_master_baptize.h"
#include "baptize.h"
#include "ifs_types.h"

uint8_t ms_bapt_state = BAPT_INIT_SERIES;


static inline void uint32_to_data(uint32_t data)
{
	union u32_arr_u {
		uint32_t l;
		uint8_t b[4];
	} *arrptr;

	arrptr = (union u32_arr_u *) &ms_datafile.msd.data;
	arrptr->l = UINT32_TO_IFS(data);
}

static void comp_write(uint32_t number, uint8_t rec)
{
	ms_datafile.msa.ln = LN_NBAPT;
	ms_datafile.msa.fn_op = OP_MSWRITE | (TTPA_SYSFILE_CONF<<2);
	ms_datafile.msa.rec = rec;
	uint32_to_data(number);
}

static void name_write(uint8_t name)
{
	ms_datafile.msa.ln = LN_NBAPT;
	ms_datafile.msa.fn_op = OP_MSWRITE | (TTPA_SYSFILE_CONF<<2);
	ms_datafile.msa.rec = 0x01;
	ms_datafile.msd.data[0] = TTPA_STATE_UNBAPT;
	ms_datafile.msd.data[1] = ttpa_conffile.ccn;
	ms_datafile.msd.data[2] = name;
	ms_datafile.msd.data[3] = LN_NBAPT;
}

static void node_compare(uint8_t rec)
{
	ms_datafile.msa.ln = LN_NBAPT;
	ms_datafile.msa.fn_op = OP_MSEXEC | (TTPA_SYSFILE_CONF<<2);
	ms_datafile.msa.rec = rec;
	ms_msdatafile.msd.data[0] = 0xff;
	ms_msd_rodl[0].f_len = 0;
}


void ms_setup_baptize(void)
{
	switch(ms_bapt_state) {
	case BAPT_INIT_SERIES: 
		lo.ll = 0x0000000000000000;
		hi.ll = 0xffffffffffffffff;
		comp_write(lo.series, 0x02);
		break;
	case BAPT_INIT_SERIAL:
		comp_write(lo.serial, 0x03);
		break;
	case BAPT_COMP_NN:
	case BAPT_COMP_CI:
		node_compare(0x02);
		break;
	case BAPT_CI_SERIES:
		calc_ci();
		comp_write(ci.series, 0x02);
		break;
	case BAPT_CI_SERIAL:
	case BAPT_NAME_SERIAL:
		comp_write(ci.serial, 0x03);
		break;
	case BAPT_NAME_SERIES:
		comp_write(ci.series, 0x02);
		break;
	case BAPT_NAME_WRITE:
		// TODO: get name
		name_write(name);
		break;
	case BAPT_NAME_ASSIGN:
		node_compare(0x01);
		break;
	}
}

void ms_callback_baptize(void)
{
	switch(ms_bapt_state) {
	case BAPT_INIT_SERIES:
		ms_bapt_state = BAPT_INIT_SERIAL;
		break;
	case BAPT_INIT_SERIAL:
		ms_bapt_state = BAPT_COMP_NN;
		break;
	case BAPT_COMP_NN:
		if(ms_msdatafile.msd.data[0] == 0xff) {
			// no new node
			ifs_bitvec_clearb(ms_datafile.ms_pending, 8, 1);
			ms_bapt_state = BAPT_INIT_SERIES;
		} else {
			ms_bapt_state = BAPT_CI_SERIES;
		}
		break;
	case BAPT_CI_SERIES:
		ms_bapt_state = BAPT_CI_SERIAL;
		break;
	case BAPT_CI_SERIAL:
		ms_bapt_state = BAPT_COMP_CI;
		break;
	case BAPT_COMP_CI:
		if(comp_ci_ge_asm(&ci.ll, &hi.ll)) {
			if(ms_msdatafile.msd.data[0] == 0xff) {
				ms_bapt_state = BAPT_NAME_WRITE;
			} else {
				ci.ll = lo.ll;
				ms_bapt_state = BAPT_NAME_SERIAL;
			}
		} else {
			if(ms_msdatafile.msd.data[0] == 0xff) {
				lo.ll = ci.ll;
			} else {
				hi.ll = ci.ll - 1;
			}
			ms_bapt_state = BAPT_CI_SERIES;
		}
		break;
	case BAPT_NAME_SERIES:
		ms_bapt_state = BAPT_NAME_SERIAL;
		break;
	case BAPT_NAME_SERIAL:
		ms_bapt_state = BAPT_NAME_WRITE;
		break;
	case BAPT_NAME_WRITE:
		ms_bapt_state = BAPT_NAME_ASSIGN;
		break;
	case BAPT_NAME_ASSIGN:
		ifs_bitvec_clearb(ms_datafile.ms_pending, 8, 1);
		ms_bapt_state = BAPT_INIT_SERIES;
		break;
	}
}
