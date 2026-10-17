#ifndef RVL_SDK_BTE_GKI_H
#define RVL_SDK_BTE_GKI_H

#include "RVL_SDK/bte/data_types.h"

typedef void TIMER_CBACK(void* p_tle);
typedef UINT32 TIMER_PARAM_TYPE;

typedef struct _tle {
    struct _tle* p_next;
    struct _tle* p_prev;
    TIMER_CBACK* p_cback;
    INT32 ticks;
    TIMER_PARAM_TYPE param;
    UINT16 event;
    UINT8 in_use;
} TIMER_LIST_ENT;

void GKI_freebuf(void* p_buf);

#endif
