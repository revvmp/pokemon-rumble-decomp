#ifndef BTE_H
#define BTE_H

#include "revolution/bte/data_types.h"
#include "revolution/bte/gki.h"
#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char BOOLEAN;

#define MAX_TRACE_LEVEL 5

#define BD_ADDR_LEN 6
typedef UINT8 BD_ADDR[BD_ADDR_LEN];
typedef UINT8* BD_ADDR_PTR;

#define LINK_KEY_LEN 16
typedef UINT8 LINK_KEY[LINK_KEY_LEN];

#define DEV_CLASS_LEN 3
typedef UINT8 DEV_CLASS[DEV_CLASS_LEN];
typedef UINT8* DEV_CLASS_PTR;

#define BD_NAME_LEN 248
typedef UINT8 BD_NAME[BD_NAME_LEN];
typedef UINT8* BD_NAME_PTR;

#define BD_FEATURES_LEN 8
typedef UINT8 BD_FEATURES[BD_FEATURES_LEN];

#define BT_DEVICE_TYPE_BREDR 0x01
#define BT_DEVICE_TYPE_BLE 0x02
#define BT_DEVICE_TYPE_DUMO 0x03
typedef UINT8 tBT_DEVICE_TYPE;

#define MAX_UUID_SIZE 16
typedef struct {
#define LEN_UUID_16 2
#define LEN_UUID_32 4
#define LEN_UUID_128 16

    UINT16 len;

    union {
        UINT16 uuid16;
        UINT32 uuid32;
        UINT8 uuid128[MAX_UUID_SIZE];
    } uu;

} tBT_UUID;

#define BTA_SUCCESS 0
#define BTA_FAILURE 1
#define BTA_PENDING 2
#define BTA_BUSY 3
#define BTA_NO_RESOURCES 4
#define BTA_WRONG_MODE 5

typedef UINT8 tBTA_STATUS;

#define BTA_RES_SERVICE_ID 0
#define BTA_SPP_SERVICE_ID 1
#define BTA_DUN_SERVICE_ID 2
#define BTA_A2DP_SOURCE_SERVICE_ID 3
#define BTA_LAP_SERVICE_ID 4
#define BTA_HSP_SERVICE_ID 5
#define BTA_HFP_SERVICE_ID 6
#define BTA_OPP_SERVICE_ID 7
#define BTA_FTP_SERVICE_ID 8
#define BTA_CTP_SERVICE_ID 9
#define BTA_ICP_SERVICE_ID 10
#define BTA_SYNC_SERVICE_ID 11
#define BTA_BPP_SERVICE_ID 12
#define BTA_BIP_SERVICE_ID 13
#define BTA_PANU_SERVICE_ID 14
#define BTA_NAP_SERVICE_ID 15
#define BTA_GN_SERVICE_ID 16
#define BTA_SAP_SERVICE_ID 17
#define BTA_A2DP_SERVICE_ID 18
#define BTA_AVRCP_SERVICE_ID 19
#define BTA_HID_SERVICE_ID 20
#define BTA_VDP_SERVICE_ID 21
#define BTA_PBAP_SERVICE_ID 22
#define BTA_HSP_HS_SERVICE_ID 23
#define BTA_HFP_HS_SERVICE_ID 24
#define BTA_MAP_SERVICE_ID 25
#define BTA_MN_SERVICE_ID 26
#define BTA_HDP_SERVICE_ID 27
#define BTA_PCE_SERVICE_ID 28

#define BTA_USER_SERVICE_ID 29
#define BTA_MAX_SERVICE_ID 30

#define BTA_FIRST_JV_SERVICE_ID (BTM_SEC_SERVICE_FIRST_EMPTY + 1)
#define BTA_LAST_JV_SERVICE_ID (BTM_SEC_MAX_SERVICES - 1)

typedef UINT8 tBTA_SERVICE_ID;

#define BTA_RES_SERVICE_MASK 0x00000001
#define BTA_SPP_SERVICE_MASK 0x00000002
#define BTA_DUN_SERVICE_MASK 0x00000004
#define BTA_FAX_SERVICE_MASK 0x00000008
#define BTA_LAP_SERVICE_MASK 0x00000010
#define BTA_HSP_SERVICE_MASK 0x00000020
#define BTA_HFP_SERVICE_MASK 0x00000040
#define BTA_OPP_SERVICE_MASK 0x00000080
#define BTA_FTP_SERVICE_MASK 0x00000100
#define BTA_CTP_SERVICE_MASK 0x00000200
#define BTA_ICP_SERVICE_MASK 0x00000400
#define BTA_SYNC_SERVICE_MASK 0x00000800
#define BTA_BPP_SERVICE_MASK 0x00001000
#define BTA_BIP_SERVICE_MASK 0x00002000
#define BTA_PANU_SERVICE_MASK 0x00004000
#define BTA_NAP_SERVICE_MASK 0x00008000
#define BTA_GN_SERVICE_MASK 0x00010000
#define BTA_SAP_SERVICE_MASK 0x00020000
#define BTA_A2DP_SERVICE_MASK 0x00040000
#define BTA_AVRCP_SERVICE_MASK 0x00080000
#define BTA_HID_SERVICE_MASK 0x00100000
#define BTA_VDP_SERVICE_MASK 0x00200000
#define BTA_PBAP_SERVICE_MASK 0x00400000
#define BTA_HSP_HS_SERVICE_MASK 0x00800000
#define BTA_HFP_HS_SERVICE_MASK 0x01000000
#define BTA_MAS_SERVICE_MASK 0x02000000
#define BTA_MN_SERVICE_MASK 0x04000000
#define BTA_HL_SERVICE_MASK 0x08000000
#define BTA_PCE_SERVICE_MASK 0x10000000

#define BTA_USER_SERVICE_MASK 0x20000000

#define BTA_ALL_SERVICE_MASK 0x1FFFFFFF

typedef UINT32 tBTA_SERVICE_MASK;

#define BTA_ID_SYS 0
#define BTA_ID_DM 1
#define BTA_ID_DM_SEARCH 2
#define BTA_ID_DM_SEC 3
#define BTA_ID_DG 4
#define BTA_ID_AG 5
#define BTA_ID_OPC 6
#define BTA_ID_OPS 7
#define BTA_ID_FTS 8
#define BTA_ID_CT 9
#define BTA_ID_FTC 10
#define BTA_ID_SS 11
#define BTA_ID_PR 12
#define BTA_ID_BIC 13
#define BTA_ID_PAN 14
#define BTA_ID_BIS 15
#define BTA_ID_ACC 16
#define BTA_ID_SC 17
#define BTA_ID_AV 18
#define BTA_ID_AVK 19
#define BTA_ID_HD 20
#define BTA_ID_CG 21
#define BTA_ID_BP 22
#define BTA_ID_HH 23
#define BTA_ID_PBS 24
#define BTA_ID_PBC 25
#define BTA_ID_JV 26
#define BTA_ID_HS 27
#define BTA_ID_MSE 28
#define BTA_ID_MCE 29
#define BTA_ID_HL 30
#define BTA_ID_GATTC 31
#define BTA_ID_GATTS 32
#define BTA_ID_BLUETOOTH_MAX 33

#define BTA_ID_FM 34
#define BTA_ID_FMTX 35

#define BTA_ID_SSR 36

#define BTA_ID_GPS 37

#define BTA_ID_PRM 38
#define BTA_ID_SYSTEM 39
#define BTA_ID_SWRAP 40
#define BTA_ID_MIP 41
#define BTA_ID_RT 42

#define BTA_ID_JV1 43
#define BTA_ID_JV2 44

#define BTA_ID_MAX (43 + BTA_DM_NUM_JV_ID)

typedef UINT8 tBTA_SYS_ID;

enum {
    BTM_SUCCESS = 0,
    BTM_CMD_STARTED,
    BTM_BUSY,
    BTM_NO_RESOURCES,
    BTM_MODE_UNSUPPORTED,
    BTM_ILLEGAL_VALUE,
    BTM_WRONG_MODE,
    BTM_UNKNOWN_ADDR,
    BTM_DEVICE_TIMEOUT,
    BTM_BAD_VALUE_RET,
    BTM_ERR_PROCESSING,
    BTM_NOT_AUTHORIZED,
    BTM_DEV_RESET,
    BTM_CMD_STORED,
    BTM_ILLEGAL_ACTION,
    BTM_DELAY_CHECK,
    BTM_SCO_BAD_LENGTH,
    BTM_SUCCESS_NO_SECURITY,
    BTM_FAILED_ON_SECURITY,
    BTM_REPEATED_ATTEMPTS
};
typedef UINT8 tBTM_STATUS;

typedef struct {
    UINT16 event;
    UINT16 len;
    UINT16 offset;
    UINT16 layer_specific;
} BT_HDR;

#define BT_HDR_SIZE (sizeof(BT_HDR))

#define BTM_SEC_NONE 0x0000
#define BTM_SEC_IN_AUTHORIZE 0x0001
#define BTM_SEC_IN_AUTHENTICATE 0x0002
#define BTM_SEC_IN_ENCRYPT 0x0004
#define BTM_SEC_OUT_AUTHORIZE 0x0008
#define BTM_SEC_OUT_AUTHENTICATE 0x0010
#define BTM_SEC_OUT_ENCRYPT 0x0020
#define BTM_SEC_BOND 0x0040
#define BTM_SEC_BOND_CONN 0x0080
#define BTM_SEC_FORCE_MASTER 0x0100
#define BTM_SEC_ATTEMPT_MASTER 0x0200
#define BTM_SEC_FORCE_SLAVE 0x0400
#define BTM_SEC_ATTEMPT_SLAVE 0x0800
#define BTM_SEC_IN_MITM 0x1000
#define BTM_SEC_OUT_MITM 0x2000

#define BTA_SEC_NONE BTM_SEC_NONE
#define BTA_SEC_AUTHORIZE (BTM_SEC_IN_AUTHORIZE)
#define BTA_SEC_AUTHENTICATE (BTM_SEC_IN_AUTHENTICATE | BTM_SEC_OUT_AUTHENTICATE)
#define BTA_SEC_ENCRYPT (BTM_SEC_IN_ENCRYPT | BTM_SEC_OUT_ENCRYPT)

typedef UINT8 tBTA_SEC;

#define BTA_DM_PAIRABLE 1
#define BTA_DM_NON_PAIRABLE 0

#define BTA_DM_CONN_ALL 0
#define BTA_DM_CONN_PAIRED 1

#define BTM_GENERAL_INQUIRY 0
#define BTM_LIMITED_INQUIRY 1
#define BTM_BR_INQUIRY_MASK 0x0f
#define BTM_BLE_INQUIRY_NONE 0x00
#define BTM_BLE_GENERAL_INQUIRY 0x10
#define BTM_BLE_LIMITED_INQUIRY 0x20
#define BTM_BLE_INQUIRY_MASK (BTM_BLE_GENERAL_INQUIRY | BTM_BLE_LIMITED_INQUIRY)

#define BTA_BLE_INQUIRY_NONE BTM_BLE_INQUIRY_NONE
#define BTA_BLE_GENERAL_INQUIRY BTM_BLE_GENERAL_INQUIRY
#define BTA_BLE_LIMITED_INQUIRY BTM_BLE_LIMITED_INQUIRY
typedef UINT8 tBTA_DM_INQ_MODE;

#define BTA_DM_INQ_CLR BTM_CLR_INQUIRY_FILTER
#define BTA_DM_INQ_DEV_CLASS BTM_FILTER_COND_DEVICE_CLASS
#define BTA_DM_INQ_BD_ADDR BTM_FILTER_COND_BD_ADDR

typedef UINT8 tBTA_DM_INQ_FILT;

typedef struct {
    DEV_CLASS dev_class;
    DEV_CLASS dev_class_mask;
} tBTA_DM_COD_COND;

typedef union {
    BD_ADDR bd_addr;
    tBTA_DM_COD_COND dev_class_cond;
} tBTA_DM_INQ_COND;

typedef struct {
    tBTA_DM_INQ_MODE mode;
    UINT8 duration;
    UINT8 max_resps;
    BOOLEAN report_dup;
    tBTA_DM_INQ_FILT filter_type;
    tBTA_DM_INQ_COND filter_cond;
} tBTA_DM_INQ;

#define BTA_DM_ENABLE_EVT 0
#define BTA_DM_DISABLE_EVT 1
#define BTA_DM_PIN_REQ_EVT 2
#define BTA_DM_AUTH_CMPL_EVT 3
#define BTA_DM_AUTHORIZE_EVT 4
#define BTA_DM_LINK_UP_EVT 5
#define BTA_DM_LINK_DOWN_EVT 6
#define BTA_DM_SIG_STRENGTH_EVT 7
#define BTA_DM_BUSY_LEVEL_EVT 8
#define BTA_DM_BOND_CANCEL_CMPL_EVT 9
#define BTA_DM_SP_CFM_REQ_EVT 10
#define BTA_DM_SP_KEY_NOTIF_EVT 11
#define BTA_DM_SP_RMT_OOB_EVT 12
#define BTA_DM_SP_KEYPRESS_EVT 13
#define BTA_DM_ROLE_CHG_EVT 14
#define BTA_DM_BLE_KEY_EVT 15
#define BTA_DM_BLE_SEC_REQ_EVT 16
#define BTA_DM_BLE_PASSKEY_NOTIF_EVT 17
#define BTA_DM_BLE_PASSKEY_REQ_EVT 18
#define BTA_DM_BLE_OOB_REQ_EVT 19
#define BTA_DM_BLE_LOCAL_IR_EVT 20
#define BTA_DM_BLE_LOCAL_ER_EVT 21
#define BTA_DM_BLE_AUTH_CMPL_EVT 22
#define BTA_DM_DEV_UNPAIRED_EVT 23
#define BTA_DM_HW_ERROR_EVT 24
typedef UINT8 tBTA_DM_SEC_EVT;

typedef struct {
    BD_ADDR bd_addr;
    tBTA_STATUS status;
} tBTA_DM_ENABLE;

typedef struct {
    BD_ADDR bd_addr;
    BD_NAME bd_name;
    DEV_CLASS dev_class;
} tBTA_DM_PIN_REQ;

#define HCI_LKEY_TYPE_COMBINATION 0x00
#define HCI_LKEY_TYPE_LOCAL_UNIT 0x01
#define HCI_LKEY_TYPE_REMOTE_UNIT 0x02
#define HCI_LKEY_TYPE_DEBUG_COMB 0x03
#define HCI_LKEY_TYPE_UNAUTH_COMB 0x04
#define HCI_LKEY_TYPE_AUTH_COMB 0x05
#define HCI_LKEY_TYPE_CHANGED_COMB 0x06

typedef struct {
    BD_ADDR bd_addr;
    BD_NAME bd_name;
    BOOLEAN key_present;
    LINK_KEY key;
    UINT8 key_type;
    BOOLEAN success;
    UINT8 fail_reason;
} tBTA_DM_AUTH_CMPL;

typedef struct {
    BD_ADDR bd_addr;
    BD_NAME bd_name;
    tBTA_SERVICE_ID service;
    DEV_CLASS dev_class;
} tBTA_DM_AUTHORIZE;

typedef struct {
    BD_ADDR bd_addr;
} tBTA_DM_LINK_UP;

#define HCI_SUCCESS 0x00
#define HCI_PENDING 0x00
#define HCI_ERR_ILLEGAL_COMMAND 0x01
#define HCI_ERR_NO_CONNECTION 0x02
#define HCI_ERR_HW_FAILURE 0x03
#define HCI_ERR_PAGE_TIMEOUT 0x04
#define HCI_ERR_AUTH_FAILURE 0x05
#define HCI_ERR_KEY_MISSING 0x06
#define HCI_ERR_MEMORY_FULL 0x07
#define HCI_ERR_CONNECTION_TOUT 0x08
#define HCI_ERR_MAX_NUM_OF_CONNECTIONS 0x09
#define HCI_ERR_MAX_NUM_OF_SCOS 0x0A
#define HCI_ERR_CONNECTION_EXISTS 0x0B
#define HCI_ERR_COMMAND_DISALLOWED 0x0C
#define HCI_ERR_HOST_REJECT_RESOURCES 0x0D
#define HCI_ERR_HOST_REJECT_SECURITY 0x0E
#define HCI_ERR_HOST_REJECT_DEVICE 0x0F
#define HCI_ERR_HOST_TIMEOUT 0x10
#define HCI_ERR_UNSUPPORTED_VALUE 0x11
#define HCI_ERR_ILLEGAL_PARAMETER_FMT 0x12
#define HCI_ERR_PEER_USER 0x13
#define HCI_ERR_PEER_LOW_RESOURCES 0x14
#define HCI_ERR_PEER_POWER_OFF 0x15
#define HCI_ERR_CONN_CAUSE_LOCAL_HOST 0x16
#define HCI_ERR_REPEATED_ATTEMPTS 0x17
#define HCI_ERR_PAIRING_NOT_ALLOWED 0x18
#define HCI_ERR_UNKNOWN_LMP_PDU 0x19
#define HCI_ERR_UNSUPPORTED_REM_FEATURE 0x1A
#define HCI_ERR_SCO_OFFSET_REJECTED 0x1B
#define HCI_ERR_SCO_INTERVAL_REJECTED 0x1C
#define HCI_ERR_SCO_AIR_MODE 0x1D
#define HCI_ERR_INVALID_LMP_PARAM 0x1E
#define HCI_ERR_UNSPECIFIED 0x1F
#define HCI_ERR_UNSUPPORTED_LMP_FEATURE 0x20
#define HCI_ERR_ROLE_CHANGE_NOT_ALLOWED 0x21
#define HCI_ERR_LMP_RESPONSE_TIMEOUT 0x22
#define HCI_ERR_LMP_ERR_TRANS_COLLISION 0x23
#define HCI_ERR_LMP_PDU_NOT_ALLOWED 0x24
#define HCI_ERR_ENCRY_MODE_NOT_ACCEPTABLE 0x25
#define HCI_ERR_UNIT_KEY_USED 0x26
#define HCI_ERR_QOS_NOT_SUPPORTED 0x27
#define HCI_ERR_INSTANT_PASSED 0x28
#define HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED 0x29
#define HCI_ERR_DIFF_TRANSACTION_COLLISION 0x2A
#define HCI_ERR_UNDEFINED_0x2B 0x2B
#define HCI_ERR_QOS_UNACCEPTABLE_PARAM 0x2C
#define HCI_ERR_QOS_REJECTED 0x2D
#define HCI_ERR_CHAN_CLASSIF_NOT_SUPPORTED 0x2E
#define HCI_ERR_INSUFFCIENT_SECURITY 0x2F
#define HCI_ERR_PARAM_OUT_OF_RANGE 0x30
#define HCI_ERR_UNDEFINED_0x31 0x31
#define HCI_ERR_ROLE_SWITCH_PENDING 0x32
#define HCI_ERR_UNDEFINED_0x33 0x33
#define HCI_ERR_RESERVED_SLOT_VIOLATION 0x34
#define HCI_ERR_ROLE_SWITCH_FAILED 0x35
#define HCI_ERR_INQ_RSP_DATA_TOO_LARGE 0x36
#define HCI_ERR_SIMPLE_PAIRING_NOT_SUPPORTED 0x37
#define HCI_ERR_HOST_BUSY_PAIRING 0x38
#define HCI_ERR_REJ_NO_SUITABLE_CHANNEL 0x39
#define HCI_ERR_CONTROLLER_BUSY 0x3A
#define HCI_ERR_UNACCEPT_CONN_INTERVAL 0x3B
#define HCI_ERR_DIRECTED_ADVERTISING_TIMEOUT 0x3C
#define HCI_ERR_CONN_TOUT_DUE_TO_MIC_FAILURE 0x3D
#define HCI_ERR_CONN_FAILED_ESTABLISHMENT 0x3E
#define HCI_ERR_MAC_CONNECTION_FAILED 0x3F

#define HCI_ERR_MAX_ERR 0x40

#define HCI_HINT_TO_RECREATE_AMP_PHYS_LINK 0xFF

typedef struct {
    BD_ADDR bd_addr;
    UINT8 status;
} tBTA_DM_LINK_DOWN;

typedef INT8 tBTA_DM_RSSI_VALUE;
typedef UINT8 tBTA_DM_LINK_QUALITY_VALUE;

#define BTA_SIG_STRENGTH_RSSI_MASK 1
#define BTA_SIG_STRENGTH_LINK_QUALITY_MASK 2

typedef UINT8 tBTA_SIG_STRENGTH_MASK;

typedef struct {
    BD_ADDR bd_addr;
    tBTA_SIG_STRENGTH_MASK mask;
    tBTA_DM_RSSI_VALUE rssi_value;
    tBTA_DM_LINK_QUALITY_VALUE link_quality_value;

} tBTA_DM_SIG_STRENGTH;

typedef struct {
    UINT8 level;
} tBTA_DM_BUSY_LEVEL;

typedef union {
    tBTA_DM_ENABLE enable;
    tBTA_DM_PIN_REQ pin_req;
    tBTA_DM_AUTH_CMPL auth_cmpl;
    tBTA_DM_AUTHORIZE authorize;
    tBTA_DM_LINK_UP link_up;
    tBTA_DM_LINK_DOWN link_down;
    tBTA_DM_SIG_STRENGTH sig_strength;
    tBTA_DM_BUSY_LEVEL busy_level;
} tBTA_DM_SEC;

typedef void(tBTA_DM_SEC_CBACK)(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* p_data);

#define BTA_DM_INQ_RES_EVT 0
#define BTA_DM_INQ_CMPL_EVT 1
#define BTA_DM_DISC_RES_EVT 2
#define BTA_DM_DISC_BLE_RES_EVT 3
#define BTA_DM_DISC_CMPL_EVT 3
#define BTA_DM_DI_DISC_CMPL_EVT 5
#define BTA_DM_SEARCH_CANCEL_CMPL_EVT 4

typedef UINT8 tBTA_DM_SEARCH_EVT;

#define BTA_DM_INQ_RES_IGNORE_RSSI BTM_INQ_RES_IGNORE_RSSI

typedef struct {
    BD_ADDR bd_addr;
    DEV_CLASS dev_class;
    INT8 rssi;
    UINT8* p_eir;
} tBTA_DM_INQ_RES;

typedef struct {
    UINT8 num_resps;
} tBTA_DM_INQ_CMPL;

typedef struct {
    BD_ADDR bd_addr;
    UINT8 num_record;
    tBTA_STATUS result;
} tBTA_DM_DI_DISC_CMPL;

typedef struct {
    BD_ADDR bd_addr;
    BD_NAME bd_name;
    tBTA_SERVICE_MASK services;
    UINT8* p_raw_data;
    UINT32 raw_data_size;
    tBT_DEVICE_TYPE device_type;
    UINT32 num_uuids;
    UINT8* p_uuid_list;
    tBTA_STATUS result;
} tBTA_DM_DISC_RES;

typedef struct {
    BD_ADDR bd_addr;
    BD_NAME bd_name;
    tBT_UUID service;
} tBTA_DM_DISC_BLE_RES;

typedef union {
    tBTA_DM_INQ_RES inq_res;
    tBTA_DM_INQ_CMPL inq_cmpl;
    tBTA_DM_DISC_RES disc_res;
    tBTA_DM_DISC_BLE_RES disc_ble_res;
    tBTA_DM_DI_DISC_CMPL di_disc;
} tBTA_DM_SEARCH;

typedef void(tBTA_DM_SEARCH_CBACK)(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* p_data);

#define BTA_HH_PROTO_RPT_MODE (0x00)
#define BTA_HH_PROTO_BOOT_MODE (0x01)
#define BTA_HH_PROTO_UNKNOWN (0xff)
typedef UINT8 tBTA_HH_PROTO_MODE;

#define BTA_HH_ENABLE_EVT 0
#define BTA_HH_DISABLE_EVT 1
#define BTA_HH_OPEN_EVT 2
#define BTA_HH_CLOSE_EVT 3
#define BTA_HH_GET_RPT_EVT 4
#define BTA_HH_SET_RPT_EVT 5
#define BTA_HH_GET_PROTO_EVT 6
#define BTA_HH_SET_PROTO_EVT 7
#define BTA_HH_GET_IDLE_EVT 8
#define BTA_HH_SET_IDLE_EVT 9
#define BTA_HH_GET_DSCP_EVT 10
#define BTA_HH_ADD_DEV_EVT 11
#define BTA_HH_RMV_DEV_EVT 12
#define BTA_HH_VC_UNPLUG_EVT 13
#define BTA_HH_UPDATE_UCD_EVT 14
#define BTA_HH_API_ERR_EVT 15

typedef UINT16 tBTA_HH_EVT;

enum {
    BTA_HH_OK,
    BTA_HH_HS_HID_NOT_READY,
    BTA_HH_HS_INVALID_RPT_ID,
    BTA_HH_HS_TRANS_NOT_SPT,
    BTA_HH_HS_INVALID_PARAM,
    BTA_HH_HS_ERROR,
    BTA_HH_ERR,
    BTA_HH_ERR_SDP,
    BTA_HH_ERR_PROTO,
    BTA_HH_ERR_DB_FULL,
    BTA_HH_ERR_TOD_UNSPT,
    BTA_HH_ERR_NO_RES,
    BTA_HH_ERR_AUTH_FAILED,
    BTA_HH_ERR_HDL
};
typedef UINT8 tBTA_HH_STATUS;

typedef struct {
    BD_ADDR bda;
    tBTA_HH_STATUS status;
    UINT8 handle;
} tBTA_HH_CONN;

typedef tBTA_HH_CONN tBTA_HH_DEV_INFO;

typedef struct {
    tBTA_HH_STATUS status;
    UINT8 handle;
} tBTA_HH_CBDATA;

typedef union {
    tBTA_HH_DEV_INFO dev_info;
    tBTA_HH_CONN conn;
    tBTA_HH_CBDATA dev_status;
} tBTA_HH;

typedef void(tBTA_HH_CBACK)(tBTA_HH_EVT event, tBTA_HH* p_data);

typedef void(tBTM_CMPL_CB)(void* p1);

typedef struct {
    UINT8 hci_version;
    UINT16 hci_revision;
    UINT8 lmp_version;
    UINT16 manufacturer;
    UINT16 lmp_subversion;
} tBTM_VERSION_INFO;

typedef struct {
    UINT16 opcode;
    UINT16 param_len;
    UINT8* p_param_buf;
} tBTM_VSC_CMPL;

#define BTM_VSC_CMPL_DATA_SIZE (BTM_MAX_VENDOR_SPECIFIC_LEN + sizeof(tBTM_VSC_CMPL))
enum { BTM_DEV_STATUS_UP, BTM_DEV_STATUS_DOWN, BTM_DEV_STATUS_CMD_TOUT };
typedef UINT8 tBTM_DEV_STATUS;

typedef void(tBTM_DEV_STATUS_CB)(tBTM_DEV_STATUS status);

typedef void(tBTM_VS_EVT_CB)(UINT8 len, UINT8* p);

typedef void(tBTM_VSC_CMPL_CB)(tBTM_VSC_CMPL* p1);

#define HID_VIRTUAL_CABLE 0x0001
#define HID_NORMALLY_CONNECTABLE 0x0002
#define HID_RECONN_INIT 0x0004
#define HID_SDP_DISABLE 0x0008
#define HID_BATTERY_POWER 0x0010
#define HID_REMOTE_WAKE 0x0020
#define HID_SUP_TOUT_AVLBL 0x0040
#define HID_SSR_MAX_LATENCY 0x0080
#define HID_SSR_MIN_TOUT 0x0100

#define HID_SEC_REQUIRED 0x8000

#define BTA_HH_VIRTUAL_CABLE HID_VIRTUAL_CABLE
#define BTA_HH_NORMALLY_CONNECTABLE HID_NORMALLY_CONNECTABLE
#define BTA_HH_RECONN_INIT HID_RECONN_INIT
#define BTA_HH_SDP_DISABLE HID_SDP_DISABLE
#define BTA_HH_BATTERY_POWER HID_BATTERY_POWER
#define BTA_HH_REMOTE_WAKE HID_REMOTE_WAKE
#define BTA_HH_SUP_TOUT_AVLBL HID_SUP_TOUT_AVLBL
#define BTA_HH_SEC_REQUIRED HID_SEC_REQUIRED
typedef UINT16 tBTA_HH_ATTR_MASK;

typedef struct desc_info {
    UINT16 dl_len;
    UINT8* dsc_list;
} tHID_DEV_DSCP_INFO;

typedef tHID_DEV_DSCP_INFO tBTA_HH_DEV_DESCR;

#define HCI_DISABLE_ALL_LM_MODES 0x0000
#define HCI_ENABLE_MASTER_SLAVE_SWITCH 0x0001
#define HCI_ENABLE_HOLD_MODE 0x0002
#define HCI_ENABLE_SNIFF_MODE 0x0004
#define HCI_ENABLE_PARK_MODE 0x0008

#define HCI_MODE_ACTIVE 0x00
#define HCI_MODE_HOLD 0x01
#define HCI_MODE_SNIFF 0x02
#define HCI_MODE_PARK 0x03

enum {
    BTM_PM_STS_ACTIVE = HCI_MODE_ACTIVE,
    BTM_PM_STS_HOLD = HCI_MODE_HOLD,
    BTM_PM_STS_SNIFF = HCI_MODE_SNIFF,
    BTM_PM_STS_PARK = HCI_MODE_PARK,
    BTM_PM_STS_SSR,
    BTM_PM_STS_PENDING,
    BTM_PM_STS_ERROR
};
typedef UINT8 tBTM_PM_STATUS;

enum {
    BTM_PM_MD_ACTIVE = BTM_PM_STS_ACTIVE,
    BTM_PM_MD_HOLD = BTM_PM_STS_HOLD,
    BTM_PM_MD_SNIFF = BTM_PM_STS_SNIFF,
    BTM_PM_MD_PARK = BTM_PM_STS_PARK,
    BTM_PM_MD_FORCE = 0x10
};
typedef UINT8 tBTM_PM_MODE;

#define BTM_PM_SET_ONLY_ID 0x80

#define BTM_PM_REG_SET 1
#define BTM_PM_REG_NOTIF 2
#define BTM_PM_DEREG 4

typedef struct {
    UINT16 max;
    UINT16 min;
    UINT16 attempt;
    UINT16 timeout;
    tBTM_PM_MODE mode;
} tBTM_PM_PWR_MD;

typedef void(tBTM_PM_STATUS_CBACK)(BD_ADDR p_bda, tBTM_PM_STATUS status, UINT16 value, UINT8 hci_status);

#define BTM_CB_EVT_RETURN_LINK_KEYS 1
#define BTM_CB_EVT_READ_STORED_LINK_KEYS 2
#define BTM_CB_EVT_WRITE_STORED_LINK_KEYS 3
#define BTM_CB_EVT_DELETE_STORED_LINK_KEYS 4

typedef struct {
    UINT8 event;

} tBTM_STORED_LINK_KEYS_EVT;

typedef struct {
    UINT8 event;
    UINT8 num_keys;

} tBTM_RETURN_LINK_KEYS_EVT;

typedef struct {
    BD_ADDR bd_addr;
    LINK_KEY link_key;

} tBTM_BD_ADDR_LINK_KEY_PAIR;

typedef struct {
    UINT8 event;
    UINT8 status;
    UINT16 max_keys;
    UINT16 read_keys;

} tBTM_READ_STORED_LINK_KEY_COMPLETE;

typedef struct {
    UINT8 event;
    UINT8 status;
    UINT8 num_keys;

} tBTM_WRITE_STORED_LINK_KEY_COMPLETE;

typedef struct {
    UINT8 event;
    UINT8 status;
    UINT16 num_keys;

} tBTM_DELETE_STORED_LINK_KEY_COMPLETE;

tBTA_STATUS BTA_EnableBluetooth(tBTA_DM_SEC_CBACK* p_cback);
tBTA_STATUS BTA_DisableBluetooth(void);

tBTA_STATUS BTA_DmAddDevice(BD_ADDR bd_addr, LINK_KEY link_key, tBTA_SERVICE_MASK trusted_mask, BOOLEAN is_trusted);
tBTA_STATUS BTA_DmRemoveDevice(BD_ADDR bd_addr);
void BTA_DmSearch(tBTA_DM_INQ* p_dm_inq, tBTA_SERVICE_MASK services, tBTA_DM_SEARCH_CBACK* p_cback);
void BTA_DmSearchCancel(void);
void BTA_DmPinReply(BD_ADDR bd_addr, BOOLEAN accept, UINT8 pin_len, UINT8* p_pin);
BOOLEAN BTA_DmIsDeviceUp(void);
void BTA_DmSetDeviceName(char* p_name);

void BTA_DmSetVisibility(UINT8, UINT8);

void BTA_HhEnable(tBTA_SEC sec_mask, tBTA_HH_CBACK* p_cback);
void BTA_HhAddDev(BD_ADDR bda, tBTA_HH_ATTR_MASK attr_mask, UINT8 sub_class, UINT8 app_id, tBTA_HH_DEV_DESCR descr);
void BTA_HhRemoveDev(UINT8 dev_handle);
void BTA_HhOpen(BD_ADDR dev_bda, tBTA_HH_PROTO_MODE mode, tBTA_SEC sec_mask);
void BTA_HhClose(UINT8 dev_handle);

void BTA_HhSendData(UINT8 dev_handle, BT_HDR* p_data);

void bta_sys_set_trace_level(UINT8 level);

void BTM_DeviceReset(tBTM_CMPL_CB* p_cb);

tBTM_STATUS BTM_EnableTestMode(void);

tBTM_STATUS BTM_ReadStoredLinkKey(BD_ADDR bd_addr, tBTM_CMPL_CB* p_cb);
tBTM_STATUS BTM_WriteStoredLinkKey(UINT8 num_keys, BD_ADDR* bd_addr, LINK_KEY* link_key, tBTM_CMPL_CB* p_cb);
tBTM_STATUS BTM_DeleteStoredLinkKey(BD_ADDR bd_addr, tBTM_CMPL_CB* p_cb);

tBTM_STATUS BTM_SetPowerMode(UINT8 pm_id, BD_ADDR remote_bda, tBTM_PM_PWR_MD* p_mode);
tBTM_STATUS BTM_SetAfhChannels(UINT8 first, UINT8 last);

tBTM_STATUS BTM_VendorSpecificCommand(UINT16 opcode, UINT8 param_len, UINT8* p_param_buf, tBTM_VSC_CMPL_CB* p_cb);

tBTM_STATUS BTM_ReadLocalVersion(tBTM_VERSION_INFO* p_vers);
tBTM_STATUS BTM_SetDeviceClass(DEV_CLASS dev_class);

tBTM_STATUS btm_remove_acl(BD_ADDR bd_addr);

tBTM_STATUS BTM_RegisterForVSEvents(tBTM_VS_EVT_CB* p_cb);
tBTM_DEV_STATUS_CB* BTM_RegisterForDeviceStatusNotif(tBTM_DEV_STATUS_CB* p_cb);

tBTM_STATUS BTM_PmRegister(UINT8 mask, UINT8* p_pm_id, tBTM_PM_STATUS_CBACK* p_cb);
tBTM_STATUS BTM_WritePageTimeout(UINT16 timeout);
void BTM_SetDefaultLinkPolicy(UINT16 settings);
void BTM_SetDefaultLinkSuperTout(UINT16 timeout);

BOOLEAN BTM_SecAddDevice(BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name, BD_FEATURES features, UINT32 trusted_mask[], LINK_KEY link_key);

void* GKI_getbuf(UINT16 size);

UINT8 L2CA_SetTraceLevel(UINT8 trace_level);

UINT8 SDP_SetTraceLevel(UINT8 new_level);

void BTA_DmSendHciReset(void);
void BTA_HhGetAclQueueInfo(void);
void BTA_Init(void);
void BTA_CleanUp(void (*p_cb)(tBTA_STATUS status));

#define SC_MAX_DEV_ENTRY_FOR_STD 10
#define SC_MAX_DEV_ENTRY_FOR_SMP 6
#define SC_MAX_DEV_ENTRY (SC_MAX_DEV_ENTRY_FOR_STD + SC_MAX_DEV_ENTRY_FOR_SMP)

struct small_dev_info {
    char devName[20];
    char at_0x14[1];
    char __pad0[0x20 - (0x14 + 0x01)];
    LINK_KEY linkKey;
    char __pad1[0x10];
};

typedef struct {
    UINT8 handle;
    UINT16 num_queued;
    UINT16 num_not_acked;
} tBTA_HH_ACL_QUEUE;

typedef struct {
    UINT16 num_avail_buffs;
    UINT16 num_buffs;
    UINT16 num_links;
    tBTA_HH_ACL_QUEUE queue_info[4];
} tBTA_HH_ACL_QUEUE_INFO;

typedef void (*tUSER_CBACK)(UINT8 err);

#ifdef __cplusplus
}
#endif

#endif
