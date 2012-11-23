
#include "ttpa.h"
#include "ttpa_lifesign.h"
#include "ttpa_sysfiles.h"

void ttpa_lifesign(void)
{
	switch(ttpa_conffile.stat) {
        case TTPA_STATE_UNSYNC:
		ttpa_ls_init();
		break;
        case TTPA_STATE_PASSIVE:
        case TTPA_STATE_UNBAPT:
        case TTPA_STATE_PASSIVE_LOCK:
		ttpa_ls_passive();
		break;
        case TTPA_STATE_ACTIVE:
		ttpa_ls_active();
		break;
	default:
		ttpa_ls_error();
	}
}
