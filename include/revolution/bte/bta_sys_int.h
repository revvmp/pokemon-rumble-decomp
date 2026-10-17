#ifndef BTE_BTA_SYS_INT_H
#define BTE_BTA_SYS_INT_H

#include <revolution/bte/data_types.h>

#include <revolution/bte/bta_sys.h>
#include <revolution/bte/ptim.h>

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct
{
	tBTA_SYS_REG		*reg[BTA_ID_MAX];
	tPTIM_CB			ptim_cb;
	UINT8				timers_disabled;
	UINT8				task_id;
	UINT8				events_disabled;
	tBTA_SYS_CONN_CBACK	*prm_cb;
	tBTA_SYS_CONN_CBACK	*ppm_cb;
	tBTA_SYS_CONN_CBACK	*compress_cb;
} tBTA_SYS_CB;

extern tBTA_SYS_CB bta_sys_cb;
extern tBTA_SYS_CFG *p_bta_sys_cfg;

#ifdef __cplusplus
	}
#endif

#endif
