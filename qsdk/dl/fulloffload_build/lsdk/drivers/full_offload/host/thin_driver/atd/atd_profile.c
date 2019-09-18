#include "atd_internal.h"


#define atd_swap_32(val) do {\
								val = ntohl(val);		\
							} while (0)
#define atd_swap_16(val) do {\
								val = ntohs(val);		\
							} while (0)
void 
atd_convert_profile(acfg_radio_vap_info_t *atd_profile)
{
	a_uint8_t i;

	atd_swap_32(atd_profile->freq);
	atd_swap_16(atd_profile->country_code);
	for (i = 0; i < atd_profile->num_vaps; i++) {
		atd_swap_32(atd_profile->vap_info[i].phymode);
	}
}
