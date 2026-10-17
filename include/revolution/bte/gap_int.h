#ifndef RVL_SDK_BTE_GAP_INT_H
#define RVL_SDK_BTE_GAP_INT_H

#include <revolution/bte/bt_target.h>
#include <revolution/bte/bt_trace.h>
#include <revolution/bte/btm_api.h>
#include <revolution/bte/gap_api.h>
#include <revolution/bte/gki.h>
#include <revolution/bte/l2c_api.h>

#define GAP_MAX_BLOCKS 2

typedef UINT8 tGAP_CCB_STATE;

enum {
    GAP_CCB_STATE_IDLE = 0,
    GAP_CCB_STATE_LISTENING = 1,
    GAP_CCB_STATE_CONN_SETUP = 2,
    GAP_CCB_STATE_CFG_SETUP = 3,
    GAP_CCB_STATE_CONNECTED = 4
};

typedef UINT8 tGAP_CCB_FLAGS;

enum {
    GAP_CCB_FLAGS_THEIR_CFG_DONE = 1 << 1,
    GAP_CCB_FLAGS_OUR_CFG_DONE = 1 << 2
};

typedef struct {
    void* p_data;
    tGAP_CALLBACK* gap_cback;
    tGAP_CALLBACK* gap_inq_rslt_cback;
    UINT16 event;
    UINT8 index;
    BOOLEAN in_use;
} tGAP_INFO;

typedef struct {
    tGAP_CALLBACK* p_cback;
    tBTM_INQ_INFO* p_cur_inq;
    tGAP_FINDADDR_RESULTS results;
    BOOLEAN in_use;
} tGAP_FINDADDR_CB;

typedef struct {
    tGAP_CCB_STATE con_state;
    tGAP_CCB_FLAGS con_flags;
    UINT8 security_flags;
    UINT16 gap_handle;
    UINT16 connection_id;
    BOOLEAN rem_addr_specified;
    BD_ADDR rem_dev_address;
    UINT16 psm;
    UINT16 rem_mtu_size;
    BUFFER_Q rx_queue;
    tGAP_CONN_CALLBACK* p_callback;
    tL2CAP_CFG_INFO cfg;
} tGAP_CCB;

typedef struct {
    tL2CAP_APPL_INFO reg_info;
    tGAP_CCB ccb_pool[GAP_MAX_CONNECTIONS];
} tGAP_CONN;

typedef struct {
    tGAP_INFO blk[GAP_MAX_BLOCKS];
    tBTM_CMPL_CB* btm_cback[GAP_MAX_BLOCKS];
    UINT8 trace_level;
    tGAP_FINDADDR_CB findaddr_cb;
    tBTM_INQ_INFO* cur_inqptr;
    tGAP_CONN conn;
} tGAP_CB;

extern tGAP_CB gap_cb;

#define GAP_TRACE_MASK(type) (TRACE_LAYER_GAP | TRACE_ORG_STACK | (type))

#define GAP_TRACE_WARNING0(m)                                                                                          \
    do {                                                                                                               \
        if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING)                                                              \
            LogMsg_0(GAP_TRACE_MASK(TRACE_TYPE_WARNING), (m));                                                         \
    } while (FALSE)

#define GAP_TRACE_EVENT0(m)                                                                                            \
    do {                                                                                                               \
        if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT)                                                                \
            LogMsg_0(GAP_TRACE_MASK(TRACE_TYPE_EVENT), (m));                                                           \
    } while (FALSE)

#define GAP_TRACE_EVENT1(m, p1)                                                                                        \
    do {                                                                                                               \
        if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT)                                                                \
            LogMsg_1(GAP_TRACE_MASK(TRACE_TYPE_EVENT), (m), (UINT32)(p1));                                             \
    } while (FALSE)

#define GAP_TRACE_EVENT2(m, p1, p2)                                                                                    \
    do {                                                                                                               \
        if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT)                                                                \
            LogMsg_2(GAP_TRACE_MASK(TRACE_TYPE_EVENT), (m), (UINT32)(p1), (UINT32)(p2));                               \
    } while (FALSE)

#define GAP_TRACE_EVENT4(m, p1, p2, p3, p4)                                                                            \
    do {                                                                                                               \
        if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT)                                                                \
            LogMsg_4(GAP_TRACE_MASK(TRACE_TYPE_EVENT), (m), (UINT32)(p1), (UINT32)(p2), (UINT32)(p3), (UINT32)(p4));   \
    } while (FALSE)

#define GAP_TRACE_EVENT6(m, p1, p2, p3, p4, p5, p6)                                                                    \
    do {                                                                                                               \
        if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT)                                                                \
            LogMsg_6(GAP_TRACE_MASK(TRACE_TYPE_EVENT), (m), (UINT32)(p1), (UINT32)(p2), (UINT32)(p3), (UINT32)(p4),    \
                     (UINT32)(p5), (UINT32)(p6));                                                                      \
    } while (FALSE)

UINT16 gap_convert_btm_status(tBTM_STATUS btm_status);
void gap_btm_cback0(void* p1);
void gap_btm_cback1(void* p1);
void gap_conn_init(void);
void gap_inq_results_cb(tBTM_INQ_RESULTS* p_results);
void gap_free_cb(tGAP_INFO* p_cb);
UINT16 gap_find_local_addr_by_name(const tBTM_BD_NAME devname, BD_ADDR bd_addr);
void gap_find_addr_name_cb(tBTM_REMOTE_DEV_NAME* p);
void gap_find_addr_inq_cb(tBTM_INQUIRY_CMPL* p);

#endif
