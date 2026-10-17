#ifndef RVL_SDK_BTE_SYS_H
#define RVL_SDK_BTE_SYS_H

#include <revolution/bte/transport.h>

typedef struct TIMER_LIST_ENT TIMER_LIST_ENT;

typedef struct {
    TIMER_LIST_ENT* p_first;
    TIMER_LIST_ENT* p_last;
    int last_ticks;
} TIMER_LIST_Q;

typedef struct {
    TIMER_LIST_Q timer_queue;
    int period;
    unsigned char timer_id;
} tPTIM_CB;

typedef struct {
    unsigned char (*evt_hdlr)(HC_BT_HDR*);
    void (*disable)(void);
} tBTA_SYS_REG;

typedef void tBTA_SYS_CONN_CBACK(unsigned char status, unsigned char id,
                                unsigned char app_id, unsigned char* peer_addr);

typedef struct {
    tBTA_SYS_REG* reg[26];
    tPTIM_CB ptim_cb;
    unsigned char timers_disabled;
    unsigned char task_id;
    unsigned char events_disabled;
    tBTA_SYS_CONN_CBACK* prm_cb;
    tBTA_SYS_CONN_CBACK* ppm_cb;
    tBTA_SYS_CONN_CBACK* compress_cb;
} tBTA_SYS_CB;

enum {
    BTA_SYS_CONN_OPEN,
    BTA_SYS_CONN_CLOSE,
    BTA_SYS_APP_OPEN,
    BTA_SYS_APP_CLOSE,
    BTA_SYS_SCO_OPEN,
    BTA_SYS_SCO_CLOSE,
    BTA_SYS_CONN_IDLE,
    BTA_SYS_CONN_BUSY
};

extern tBTA_SYS_CB bta_sys_cb;

void bta_sys_rm_register(tBTA_SYS_CONN_CBACK* p_cback);
void bta_sys_compress_register(tBTA_SYS_CONN_CBACK* p_cback);
void bta_sys_pm_register(tBTA_SYS_CONN_CBACK* p_cback);
void bta_sys_conn_open(unsigned char id, unsigned char app_id, unsigned char* peer_addr);
void bta_sys_conn_close(unsigned char id, unsigned char app_id, unsigned char* peer_addr);
void bta_sys_sco_close(unsigned char id, unsigned char app_id, unsigned char* peer_addr);
void bta_sys_idle(unsigned char id, unsigned char app_id, unsigned char* peer_addr);
void bta_sys_busy(unsigned char id, unsigned char app_id, unsigned char* peer_addr);

#endif
