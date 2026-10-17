#ifndef BTE_BTA_HH_CO_H
#define BTE_BTA_HH_CO_H



#include <revolution/bte/bta_hh_api.h>

#ifdef __cplusplus
	extern "C" {
#endif

extern void bta_hh_co_open(UINT8 dev_handle, UINT8 sub_class, UINT16 attr_mask,
                           UINT8 app_id);
extern void bta_hh_co_data(UINT8 dev_handle, UINT8 *p_rpt, UINT16 len,
                           tBTA_HH_PROTO_MODE mode, UINT8 sub_class,
                           UINT8 app_id);
extern void bta_hh_co_close(UINT8 dev_handle, UINT8 app_id);

#ifdef __cplusplus
	}
#endif

#endif
