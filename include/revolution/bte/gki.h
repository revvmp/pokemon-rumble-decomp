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

enum {
    GKI_POOL_ID_0 = 0,
    GKI_POOL_ID_1 = 1,
    GKI_POOL_ID_2 = 2,
    GKI_POOL_ID_3 = 3
};

typedef struct {
    void* p_first;
    void* p_last;
    UINT16 count;
} BUFFER_Q;

void* GKI_getbuf(UINT16);
void* GKI_getpoolbuf(UINT8 pool_id);
void GKI_freebuf(void* p_buf);
void GKI_init_q(BUFFER_Q* p_q);
void GKI_enqueue(BUFFER_Q* p_q, void* p_buf);
void* GKI_dequeue(BUFFER_Q* p_q);

#ifdef __cplusplus
}
#endif

#endif
