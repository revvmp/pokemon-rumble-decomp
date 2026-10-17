#ifndef RVL_SDK_BTE_GKI_H
#define RVL_SDK_BTE_GKI_H

#include <RVL_SDK/bte/data_types.h>

typedef void TIMER_CBACK(void* p_tle);

typedef struct _tle {
    struct _tle* p_next;
    struct _tle* p_prev;
    TIMER_CBACK* p_cback;
    INT32 ticks;
    UINT32 param;
    UINT16 event;
    UINT8 in_use;
} TIMER_LIST_ENT;

typedef struct {
    TIMER_LIST_ENT* p_first;
    TIMER_LIST_ENT* p_last;
    INT32 last_ticks;
} TIMER_LIST_Q;

void* GKI_getbuf(UINT16 size);
void GKI_freebuf(void* p_buf);

#endif
