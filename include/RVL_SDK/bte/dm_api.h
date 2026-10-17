#ifndef RVL_SDK_BTE_DM_API_H
#define RVL_SDK_BTE_DM_API_H

#include <revolution/types.h>
#include <RVL_SDK/bte/transport.h>

typedef u8 BD_ADDR[6];
typedef u8 LINK_KEY[16];

enum {
    BTA_DM_API_DISABLE_EVT = 0x101,
    BTA_DM_API_SET_NAME_EVT,
    BTA_DM_API_SET_VISIBILITY_EVT,
    BTA_DM_API_PIN_REPLY_EVT = 0x107,
    BTA_DM_API_RESET_HCI = 0x10C,
    BTA_DM_API_SEARCH_EVT = 0x200,
    BTA_DM_API_SEARCH_CANCEL_EVT,
    BTA_SUCCESS = 0,
    BTA_FAILURE = 1,
    BTA_MAX_SERVICE_ID = 23,
    BTM_SEC_ARRAY_BITS = 32,
    BTA_DM_MESSAGE_SIZE = 0x110
};

typedef struct {
    u8 dev_class[3];
    u8 dev_class_mask[3];
} tBTA_DM_COD_COND;

typedef union {
    BD_ADDR bd_addr;
    tBTA_DM_COD_COND dev_class_cond;
} tBTA_DM_INQ_COND;

typedef struct {
    u8 mode;
    u8 duration;
    u8 max_resps;
    u8 filter_type;
    tBTA_DM_INQ_COND filter_cond;
} tBTA_DM_INQ;

typedef void tBTA_DM_SEARCH_CBACK(u8 event, void* p_data);

typedef struct {
    HC_BT_HDR hdr;
    char name[32];
} tBTA_DM_API_SET_NAME;

typedef struct {
    HC_BT_HDR hdr;
    u8 disc_mode;
    u8 conn_mode;
} tBTA_DM_API_SET_VISIBILITY;

typedef struct {
    HC_BT_HDR hdr;
    tBTA_DM_INQ inq_params;
    u32 services;
    tBTA_DM_SEARCH_CBACK* p_cback;
} tBTA_DM_API_SEARCH;

typedef struct {
    HC_BT_HDR hdr;
    BD_ADDR bd_addr;
    u8 accept;
    u8 pin_len;
    u8 p_pin[16];
} tBTA_DM_API_PIN_REPLY;

void BTA_DisableBluetooth(void);
u8 BTA_DmIsDeviceUp(void);
void BTA_DmSetDeviceName(char* p_name);
void BTA_DmSetVisibility(u8 disc_mode, u8 conn_mode);
void BTA_DmSearch(tBTA_DM_INQ* p_dm_inq, u32 services, tBTA_DM_SEARCH_CBACK* p_cback);
void BTA_DmSearchCancel(void);
void BTA_DmPinReply(BD_ADDR bd_addr, u8 accept, u8 pin_len, u8* p_pin);
u8 BTA_DmAddDevice(BD_ADDR bd_addr, LINK_KEY link_key, u32 trusted_mask, u8 is_trusted);
u8 BTA_DmRemoveDevice(BD_ADDR bd_addr);
void BTA_DmSendHciReset(void);

#endif
