#ifndef RVL_SDK_BTE_BTU_H
#define RVL_SDK_BTE_BTU_H

#include "RVL_SDK/bte/gki.h"

enum {
    BTU_TTYPE_USER_FUNC = 22
};

void btu_start_timer(TIMER_LIST_ENT* p_tle, UINT16 type, UINT32 timeout);
void btu_stop_timer(TIMER_LIST_ENT* p_tle);

#endif
