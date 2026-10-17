#ifndef BTE_PTIM_H
#define BTE_PTIM_H

#include <revolution/bte/data_types.h>

#include <revolution/bte/gki.h>

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct
{
	TIMER_LIST_Q	timer_queue;
	INT32			period;
	UINT8			timer_id;
} tPTIM_CB;

void ptim_init(tPTIM_CB *p_cb, UINT16 period, UINT8 timer_id);
void ptim_timer_update(tPTIM_CB *p_cb);
void ptim_start_timer(tPTIM_CB *p_cb, TIMER_LIST_ENT *p_tle, UINT16 type,
                      INT32 timeout);
void ptim_stop_timer(tPTIM_CB *p_cb, TIMER_LIST_ENT *p_tle);

#ifdef __cplusplus
	}
#endif

#endif
