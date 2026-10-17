#ifndef RVL_SDK_BTE_BTA_SYS_H
#define RVL_SDK_BTE_BTA_SYS_H

#include <revolution/bte/bt_types.h>
#include <revolution/bte/gki.h>

#define BTA_SYS_EVT_START(id) ((id) << 8)

typedef UINT8 tBTA_SYS_ID;

enum {
    BTA_ID_SYS = 0,
    BTA_ID_DM = 1,
    BTA_ID_DM_SEARCH = 2,
    BTA_ID_HH = 23,
    BTA_ID_MAX = 26
};

typedef UINT8 tBTA_SYS_CONN_STATUS;

typedef void tBTA_SYS_CONN_CBACK(tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr);

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

typedef BOOLEAN tBTA_SYS_EVT_HDLR(BT_HDR *p_msg);
typedef void tBTA_SYS_DISABLE(void);
typedef struct
{
	tBTA_SYS_EVT_HDLR	*evt_hdlr;
	tBTA_SYS_DISABLE	*disable;
} tBTA_SYS_REG;

typedef struct
{
	UINT16	mbox_evt;
	UINT8	mbox;
	UINT8	timer;
	UINT8	trace_level;
} tBTA_SYS_CFG;

extern UINT8 appl_trace_level;

void bta_sys_rm_register(tBTA_SYS_CONN_CBACK* p_cback);
void bta_sys_compress_register(tBTA_SYS_CONN_CBACK* p_cback);
void bta_sys_sendmsg(void* p_msg);
void bta_sys_disable(void);
void bta_sys_start_timer(TIMER_LIST_ENT* p_tle, UINT16 type, INT32 timeout);
void bta_sys_stop_timer(TIMER_LIST_ENT* p_tle);

void bta_sys_pm_register(tBTA_SYS_CONN_CBACK* p_cback);
void bta_sys_conn_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_conn_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_sco_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_idle(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_busy(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
void bta_sys_register(UINT8 id, const tBTA_SYS_REG* p_reg);
void bta_sys_init(void);
void bta_sys_event(BT_HDR* p_msg);
void bta_sys_timer_update(void);
void bta_sys_disable_timers(void);
void bta_sys_set_trace_level(UINT8 level);

#endif
