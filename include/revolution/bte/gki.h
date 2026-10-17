#ifndef GKI_H
#define GKI_H

#include "revolution/bte/data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void TIMER_CBACK(void* p_tle);
typedef UINT32 TIMER_PARAM_TYPE;

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

void* GKI_getbuf(UINT16);
void GKI_freebuf(void* p_buf);

#ifdef __cplusplus
}
#endif

#endif
