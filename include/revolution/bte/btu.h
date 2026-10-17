#ifndef RVL_SDK_BTE_BTU_H
#define RVL_SDK_BTE_BTU_H

#include <revolution/bte/bt_types.h>
#include <revolution/bte/gki.h>

enum {
    BTU_TTYPE_USER_FUNC = 22
};

extern const BD_ADDR BT_BD_ANY;

void btu_start_timer(TIMER_LIST_ENT* p_tle, UINT16 type, UINT32 timeout);
void btu_stop_timer(TIMER_LIST_ENT* p_tle);

#endif
