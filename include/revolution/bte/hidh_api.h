#ifndef RVL_SDK_BTE_HIDH_API_H
#define RVL_SDK_BTE_HIDH_API_H

#include <revolution/bte/hiddefs.h>

enum {
    HID_SDP_NO_SERV_UUID = SDP_ILLEGAL_PARAMETER + 1,
    HID_SDP_MANDATORY_MISSING
};

enum {
    HID_VIRTUAL_CABLE = 1 << 0,
    HID_NORMALLY_CONNECTABLE = 1 << 1,
    HID_RECONN_INIT = 1 << 2,
    HID_SDP_DISABLE = 1 << 3,
    HID_BATTERY_POWER = 1 << 4,
    HID_REMOTE_WAKE = 1 << 5,
    HID_SUP_TOUT_AVLBL = 1 << 6
};

typedef void tHID_HOST_SDP_CALLBACK(UINT16 result, UINT16 attr_mask, tHID_DEV_SDP_INFO* sdp_rec);
typedef void tHID_HOST_DEV_CALLBACK(UINT8 dev_handle, UINT8 event, UINT32 data, BT_HDR* p_buf);

tHID_STATUS HID_HostGetSDPRecord(BD_ADDR addr, tSDP_DISCOVERY_DB* p_db, UINT32 db_len, tHID_HOST_SDP_CALLBACK* sdp_cback);
void HID_HostInit(void);
tHID_STATUS HID_HostRegister(tHID_HOST_DEV_CALLBACK* dev_cback);
tHID_STATUS HID_HostDeregister(void);
tHID_STATUS HID_HostAddDev(BD_ADDR addr, UINT16 attr_mask, UINT8* handle);
tHID_STATUS HID_HostRemoveDev(UINT8 dev_handle);
tHID_STATUS HID_HostOpenDev(UINT8 dev_handle);
tHID_STATUS HID_HostWriteDev(UINT8 dev_handle, UINT8 t_type, UINT8 param, UINT16 data, UINT8 report_id, BT_HDR* pbuf);
tHID_STATUS HID_HostCloseDev(UINT8 dev_handle);
tHID_STATUS HID_HostSetSecurityLevel(char* serv_name, UINT8 sec_lvl);

#endif
