#ifndef RVL_SDK_BTE_BTU_H
#define RVL_SDK_BTE_BTU_H

#include <revolution/bte/bt_types.h>
#include <revolution/bte/gki.h>

enum {
    BTU_TTYPE_SDP = 5
};

enum {
    BTU_TTYPE_RFCOMM_MFC = 11,
    BTU_TTYPE_RFCOMM_PORT = 12
};

enum {
    BTU_TTYPE_USER_FUNC = 22
};

#define BTU_MAX_REG_TIMER 2
#define BTU_MAX_REG_EVENT 6
#define BTU_DEFAULT_DATA_SIZE 0x2A0

typedef void tBTU_TIMER_CALLBACK(TIMER_LIST_ENT* p_tle);
typedef void tBTU_EVENT_CALLBACK(BT_HDR* p_hdr);

typedef struct {
    TIMER_LIST_ENT* p_tle;
    tBTU_TIMER_CALLBACK* timer_cb;
} tBTU_TIMER_REG;

typedef struct {
    UINT16 event_range;
    tBTU_EVENT_CALLBACK* event_cb;
} tBTU_EVENT_REG;

typedef struct {
    tBTU_TIMER_REG timer_reg[BTU_MAX_REG_TIMER];
    tBTU_EVENT_REG event_reg[BTU_MAX_REG_EVENT];
    TIMER_LIST_Q timer_queue;
    TIMER_LIST_ENT cmd_cmpl_timer;
    BUFFER_Q cmd_xmit_q;
    BUFFER_Q cmd_cmpl_q;
    UINT16 hcit_acl_data_size;
    UINT16 hcit_acl_pkt_size;
    UINT16 controller_cmd_window;
    BOOLEAN reset_complete;
} tBTU_CB;

extern tBTU_CB btu_cb;
extern const BD_ADDR BT_BD_ANY;

void btu_start_timer(TIMER_LIST_ENT* p_tle, UINT16 type, UINT32 timeout);
void btu_stop_timer(TIMER_LIST_ENT* p_tle);
void btu_hcif_send_cmd(BT_HDR* p_buf);
void btu_init_core(void);
void BTE_Init(void);

#endif
