#ifndef RVL_SDK_BTE_BTA_SYS_H
#define RVL_SDK_BTE_BTA_SYS_H

#include <revolution/bte/bt_types.h>
#include <revolution/bte/gki.h>

#define BTA_SYS_EVT_START(id) ((id) << 8)

typedef UINT8 tBTA_SYS_ID;

enum {
    BTA_ID_SYS = 0,
    BTA_ID_DM = 1,
    BTA_ID_DM_SEARCH = 2
};

typedef UINT8 tBTA_SYS_CONN_STATUS;

typedef void tBTA_SYS_CONN_CBACK(tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr);

extern UINT8 appl_trace_level;

void bta_sys_rm_register(tBTA_SYS_CONN_CBACK* p_cback);
void bta_sys_compress_register(tBTA_SYS_CONN_CBACK* p_cback);
void bta_sys_sendmsg(void* p_msg);
void bta_sys_disable(void);
void bta_sys_start_timer(TIMER_LIST_ENT* p_tle, UINT16 type, INT32 timeout);
void bta_sys_stop_timer(TIMER_LIST_ENT* p_tle);

#endif
