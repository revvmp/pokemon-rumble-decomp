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

#define GKI_SUCCESS 0
#define GKI_FAILURE 1
#define GKI_INVALID_TASK 240
#define GKI_INVALID_POOL 0xFF
#define GKI_RESTRICTED_POOL 1

#define GKI_BUF2_SIZE 660
#define GKI_BUF3_SIZE 1800
#define GKI_MAX_BUF_SIZE GKI_BUF3_SIZE

typedef struct {
    void* p_first;
    void* p_last;
    UINT16 count;
} BUFFER_Q;

void* GKI_getbuf(UINT16);
UINT32 GKI_get_tick_count(void);
void* GKI_getpoolbuf(UINT8 pool_id);
void GKI_freebuf(void* p_buf);
UINT16 GKI_get_buf_size(void* p_buf);
UINT8 GKI_create_pool(UINT16 size, UINT16 count, UINT8 permission, void* p_mem_pool);
void GKI_delete_pool(UINT8 pool_id);
void GKI_init_q(BUFFER_Q* p_q);
void GKI_enqueue(BUFFER_Q* p_q, void* p_buf);
void GKI_enqueue_head(BUFFER_Q* p_q, void* p_buf);
void* GKI_dequeue(BUFFER_Q* p_q);
BOOLEAN GKI_queue_is_empty(BUFFER_Q* p_q);
void* GKI_remove_from_queue(BUFFER_Q* p_q, void* p_buf);
void* GKI_getfirst(BUFFER_Q* p_q);
void* GKI_getnext(void* p_buf);

#define EVENT_MASK(evt) ((UINT16)(1 << (evt)))
enum {
    TASK_MBOX_0 = 0,
    TASK_MBOX_2 = 2,
    TASK_MBOX_0_EVT_MASK = 1,
    TASK_MBOX_1_EVT_MASK = 2,
    TASK_MBOX_2_EVT_MASK = 4,
    TASK_MBOX_3_EVT_MASK = 8,
    TIMER_0_EVT_MASK = 0x10,
    TIMER_1_EVT_MASK = 0x20,
    APPL_EVT_7 = 15
};
void GKI_disable(void);
void GKI_enable(void);
void* GKI_read_mbox(UINT8 mbox);
UINT8 GKI_get_taskid(void);
void GKI_send_msg(UINT8 task_id, UINT8 mbox, void* p_msg);
void GKI_init_timer_list(TIMER_LIST_Q* p_queue);
UINT16 GKI_update_timer_list(TIMER_LIST_Q* p_queue, INT32 elapsed);
void GKI_add_to_timer_list(TIMER_LIST_Q* p_queue, TIMER_LIST_ENT* p_entry);
void GKI_remove_from_timer_list(TIMER_LIST_Q* p_queue, TIMER_LIST_ENT* p_entry);
void GKI_start_timer(UINT8 timer, INT32 ticks, BOOLEAN continuous);
void GKI_stop_timer(UINT8 timer);

#define GKI_MS_TO_TICKS(x) ((x) / 10)

void GKI_init(void);
void GKI_shutdown(void);
void GKI_run(void* p_task_id);
void GKI_exit_task(UINT8 task_id);
void GKI_delay(UINT32 timeout);
UINT8 GKI_send_event(UINT8 task_id, UINT16 event);
void GKI_exception(UINT16 code, char* msg);
void* GKI_os_malloc(UINT32 size);
void GKI_os_free(void* p_mem);
void* GKI_find_buf_start(void* p_user_area);
UINT8 GKI_set_pool_permission(UINT8 pool_id, UINT8 permission);
void GKI_sched_lock(void);
void GKI_sched_unlock(void);

#ifdef __cplusplus
}
#endif

#endif
