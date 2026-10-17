#include <revolution/bte/hcidefs.h>
#include <revolution/bte/sdp_api.h>
#include <revolution/bte/sdpdefs.h>
#include <revolution/bte/wbt_api.h>

BOOLEAN WBT_ExtCreateRecord(void) {
    tSDP_DI_GET_RECORD get_device_info;
    UINT32 sdp_record_handle = 0;
    tSDP_DI_RECORD* p_set_info;
    UINT8 temp_u16[2];
    UINT16 list = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;

    if (SDP_GetLocalDiRecord(&get_device_info, &sdp_record_handle) != SDP_SUCCESS) {
        p_set_info = &get_device_info.rec;

        memset(p_set_info, 0, sizeof(*p_set_info));
        p_set_info->vendor = LMP_COMPID_BROADCOM;
        p_set_info->vendor_id_source = DI_VENDOR_ID_SOURCE_BTSIG;
        p_set_info->primary_record = TRUE;

        if (SDP_SetLocalDiRecord(p_set_info, &sdp_record_handle) != SDP_SUCCESS) {
            sdp_record_handle = 0;
            return FALSE;
        }
    }

    SDP_AddUuidSequence(sdp_record_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &list);

    temp_u16[0] = 0;
    temp_u16[1] = 1;
    SDP_AddAttribute(sdp_record_handle, ATTR_ID_EXT_BRCM_VERSION, UINT_DESC_TYPE, sizeof(temp_u16), temp_u16);

    return TRUE;
}
