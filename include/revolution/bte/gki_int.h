#ifndef RVL_SDK_BTE_GKI_INT_H
#define RVL_SDK_BTE_GKI_INT_H

#include <revolution/bte/gki.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BTU_TASK 2
#define PPC_TASK 2
#define GKI_MAX_TASKS 8
#define TICKS_PER_SEC 100

#define GKI_BUF0_SIZE 64
#define GKI_BUF0_MAX 48
#define GKI_BUF1_SIZE 128
#define GKI_BUF1_MAX 26
#define GKI_BUF2_MAX 45
#define GKI_BUF3_MAX 30
#define GKI_BUF4_SIZE 0x2000
#define GKI_BUF4_MAX 9

#define GKI_NUM_FIXED_BUF_POOLS 5
#define GKI_NUM_TOTAL_BUF_POOLS 9
#define GKI_DEF_BUFPOOL_PERM_MASK 0xFFF0

#define NUM_TASK_MBOX 4
#define MAX_INT_STATE 20

#define ALIGN_POOL(pl_size) ((((pl_size) + 3) / sizeof(UINT32)) * sizeof(UINT32))
#define BUFFER_HDR_SIZE (sizeof(BUFFER_HDR_T))
#define BUFFER_PADDING_SIZE (sizeof(BUFFER_HDR_T) + sizeof(UINT32))
#define MAX_USER_BUF_SIZE ((UINT16)0xFFFF - BUFFER_PADDING_SIZE)
#define MAGIC_NO 0xDDBADDBA

enum {
    TIMER_0 = 0,
    TIMER_1 = 1
};

enum {
    TASK_DEAD = 0,
    TASK_READY = 1
};

enum {
    BUF_STATUS_FREE = 0,
    BUF_STATUS_UNLINKED = 1,
    BUF_STATUS_QUEUED = 2
};

enum {
    GKI_ERROR_BUF_SIZE_ZERO = 0xFFF6,
    GKI_ERROR_BUF_SIZE_TOOBIG = 0xFFF7,
    GKI_ERROR_DELETE_POOL_BAD_QID = 0xFFF8,
    GKI_ERROR_ENQUEUE_BUF_LINKED = 0xFFF9,
    GKI_ERROR_SEND_MSG_BUF_LINKED = 0xFFFA,
    GKI_ERROR_SEND_MSG_BAD_DEST = 0xFFFB,
    GKI_ERROR_FREEBUF_BUF_LINKED = 0xFFFC,
    GKI_ERROR_FREEBUF_BAD_QID = 0xFFFD,
    GKI_ERROR_BUF_CORRUPTED = 0xFFFF
};

typedef struct _buffer_hdr {
    struct _buffer_hdr* p_next;
    UINT8 q_id;
    UINT8 task_id;
    UINT8 status;
    UINT8 type;
} BUFFER_HDR_T;

typedef struct _free_queue {
    BUFFER_HDR_T* p_first;
    BUFFER_HDR_T* p_last;
    UINT16 size;
    UINT16 total;
    UINT16 cur_cnt;
    UINT16 max_cnt;
} FREE_QUEUE_T;

typedef struct {
    UINT8 bufpool0[(ALIGN_POOL(GKI_BUF0_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF0_MAX];
    UINT8 bufpool1[(ALIGN_POOL(GKI_BUF1_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF1_MAX];
    UINT8 bufpool2[(ALIGN_POOL(GKI_BUF2_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF2_MAX];
    UINT8 bufpool3[(ALIGN_POOL(GKI_BUF3_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF3_MAX];
    UINT8 bufpool4[(ALIGN_POOL(GKI_BUF4_SIZE) + BUFFER_PADDING_SIZE) * GKI_BUF4_MAX];
    UINT8* OSStack[GKI_MAX_TASKS];
    UINT16 OSStackSize[GKI_MAX_TASKS];
    INT8* OSTName[GKI_MAX_TASKS];
    UINT8 OSRdyTbl[GKI_MAX_TASKS];
    UINT16 OSWaitEvt[GKI_MAX_TASKS];
    UINT16 OSWaitForEvt[GKI_MAX_TASKS];
    UINT32 OSTicks;
    UINT32 OSIdleCnt;
    INT16 OSDisableNesting;
    INT16 OSLockNesting;
    INT16 OSIntNesting;
    INT32 OSTicksTilExp;
    INT32 OSNumOrigTicks;
    INT32 OSWaitTmr[GKI_MAX_TASKS];
    INT32 OSTaskTmr0[GKI_MAX_TASKS];
    INT32 OSTaskTmr0R[GKI_MAX_TASKS];
    INT32 OSTaskTmr1[GKI_MAX_TASKS];
    INT32 OSTaskTmr1R[GKI_MAX_TASKS];
    BUFFER_HDR_T* OSTaskQFirst[GKI_MAX_TASKS][NUM_TASK_MBOX];
    BUFFER_HDR_T* OSTaskQLast[GKI_MAX_TASKS][NUM_TASK_MBOX];
    FREE_QUEUE_T freeq[GKI_NUM_TOTAL_BUF_POOLS];
    UINT8* pool_start[GKI_NUM_TOTAL_BUF_POOLS];
    UINT8* pool_end[GKI_NUM_TOTAL_BUF_POOLS];
    UINT16 pool_size[GKI_NUM_TOTAL_BUF_POOLS];
    void* p_user_mempool;
    UINT16 pool_access_mask;
    UINT8 pool_list[GKI_NUM_TOTAL_BUF_POOLS];
    UINT8 curr_total_no_of_pools;
    BOOLEAN timer_nesting;
} tGKI_COM_CB;

typedef struct {
    UINT8 int_index;
    int int_state[MAX_INT_STATE];
} tGKI_OS;

typedef struct {
    tGKI_OS os;
    tGKI_COM_CB com;
} tGKI_CB;

extern tGKI_CB gki_cb;

void gki_buffer_init(void);
void gki_timers_init(void);
void gki_adjust_timer_count(INT32 ticks);
BOOLEAN gki_chk_buf_damage(void* p_buf);

#ifdef __cplusplus
}
#endif

#endif
