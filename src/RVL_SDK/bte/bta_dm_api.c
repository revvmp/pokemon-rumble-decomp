#include <revolution/bte/dm_api.h>

void* GKI_getbuf(u16 size);
void GKI_sched_lock(void);
void GKI_sched_unlock(void);
void bta_sys_sendmsg(void* p_msg);
void bdcpy(BD_ADDR destination, const BD_ADDR source);
u8 BTM_IsDeviceUp(void);
u8 BTM_SecAddDevice(BD_ADDR bd_addr, void* p_dev_class, void* p_bd_name, void* p_features, u32* p_trusted_mask, LINK_KEY link_key);
u8 BTM_SecDeleteDevice(BD_ADDR bd_addr);
void* memset(void* destination, int value, unsigned long size);
void* memcpy(void* destination, const void* source, unsigned long size);
char* strncpy(char* destination, const char* source, unsigned long size);
extern const u32 bta_service_id_to_btm_srv_id_lkup_tbl[];

void BTA_DisableBluetooth(void) {
    HC_BT_HDR* p_msg;
    if ((p_msg = GKI_getbuf(sizeof *p_msg)) != NULL) {
        p_msg->event = BTA_DM_API_DISABLE_EVT;
        bta_sys_sendmsg(p_msg);
    }
}

u8 BTA_DmIsDeviceUp(void) {
    u8 status;
    GKI_sched_lock();
    status = BTM_IsDeviceUp();
    GKI_sched_unlock();
    return status;
}

void BTA_DmSetDeviceName(char* p_name) {
    tBTA_DM_API_SET_NAME* p_msg;
    if ((p_msg = GKI_getbuf(sizeof *p_msg)) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_NAME_EVT;
        strncpy(p_msg->name, p_name, sizeof p_msg->name);
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmSetVisibility(u8 disc_mode, u8 conn_mode) {
    tBTA_DM_API_SET_VISIBILITY* p_msg;
    if ((p_msg = GKI_getbuf(BTA_DM_MESSAGE_SIZE)) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_VISIBILITY_EVT;
        p_msg->disc_mode = disc_mode;
        p_msg->conn_mode = conn_mode;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmSearch(tBTA_DM_INQ* p_dm_inq, u32 services, tBTA_DM_SEARCH_CBACK* p_cback) {
    tBTA_DM_API_SEARCH* p_msg;
    if ((p_msg = GKI_getbuf(sizeof *p_msg)) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SEARCH_EVT;
        memcpy(&p_msg->inq_params, p_dm_inq, sizeof p_msg->inq_params);
        p_msg->services = services;
        p_msg->p_cback = p_cback;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmSearchCancel(void) {
    HC_BT_HDR* p_msg;
    if ((p_msg = GKI_getbuf(sizeof *p_msg)) != NULL) {
        p_msg->event = BTA_DM_API_SEARCH_CANCEL_EVT;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmPinReply(BD_ADDR bd_addr, u8 accept, u8 pin_len, u8* p_pin) {
    tBTA_DM_API_PIN_REPLY* p_msg;
    if ((p_msg = GKI_getbuf(sizeof *p_msg)) != NULL) {
        p_msg->hdr.event = BTA_DM_API_PIN_REPLY_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->accept = accept;
        if (accept) {
            p_msg->pin_len = pin_len;
            memcpy(p_msg->p_pin, p_pin, pin_len);
        }
        bta_sys_sendmsg(p_msg);
    }
}

u8 BTA_DmAddDevice(BD_ADDR bd_addr, LINK_KEY link_key, u32 trusted_mask, u8 is_trusted) {
    u8 status;
    u32 trusted_services_mask[2];
    u8 index = 0;
    u8 btm_mask_index = 0;
    memset(trusted_services_mask, 0, sizeof trusted_services_mask);
    if (is_trusted) {
        for (; trusted_mask != 0 && index < BTA_MAX_SERVICE_ID; ++index) {
            if (trusted_mask & (1 << index)) {
                btm_mask_index = bta_service_id_to_btm_srv_id_lkup_tbl[index] / BTM_SEC_ARRAY_BITS;
                trusted_services_mask[btm_mask_index] |= 1 << (bta_service_id_to_btm_srv_id_lkup_tbl[index] - btm_mask_index * BTM_SEC_ARRAY_BITS);
                trusted_mask &= ~(1 << index);
            }
        }
    }
    GKI_sched_lock();
    status = BTM_SecAddDevice(bd_addr, NULL, NULL, NULL, trusted_services_mask, link_key);
    GKI_sched_unlock();
    if (status)
        return BTA_SUCCESS;
    else
        return BTA_FAILURE;
}

u8 BTA_DmRemoveDevice(BD_ADDR bd_addr) {
    u8 status;
    GKI_sched_lock();
    status = BTM_SecDeleteDevice(bd_addr);
    GKI_sched_unlock();
    if (status)
        return BTA_SUCCESS;
    else
        return BTA_FAILURE;
}

void BTA_DmSendHciReset(void) {
    HC_BT_HDR* p_msg;
    if ((p_msg = GKI_getbuf(sizeof *p_msg)) != NULL) {
        p_msg->event = BTA_DM_API_RESET_HCI;
        bta_sys_sendmsg(p_msg);
    }
}
