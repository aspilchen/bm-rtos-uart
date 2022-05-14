// prm.h
// Hardware Abstraction Module: prm_rstst

#ifndef PRM_H
#define PRM_H

#include <stdbool.h>

enum PrmRststEvent {
	ICEPICK_RST         = 0x00000200
	,EXTERNAL_WARM_RST  = 0x00000020
	,WDT1_RST           = 0x00000010
	,GLOBAL_WARM_SW_RST = 0x00000002
	,GLOBAL_COLD_RST    = 0x00000001
	,PRM_N_EVENTS       = 6
};

bool prm_isSet(const enum PrmRststEvent resetTrigger);
void prm_clear(void);

#endif