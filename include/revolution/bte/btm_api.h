#ifndef RVL_SDK_BTE_BTM_API_H
#define RVL_SDK_BTE_BTM_API_H

#include <revolution/bte/bt_target.h>
#include <revolution/bte/bt_types.h>
#include <revolution/bte/hcidefs.h>

#define BTM_SEC_ARRAY_BITS 32
#define BTM_SEC_MAX_SERVICES 46
#define BTM_SEC_SERVICE_ARRAY_SIZE \
    (((UINT32)BTM_SEC_MAX_SERVICES / BTM_SEC_ARRAY_BITS) + (((UINT32)BTM_SEC_MAX_SERVICES % BTM_SEC_ARRAY_BITS) ? 1 : 0))
#define BTM_SEC_TRUST_ALL 0xffffffff

#define BTM_SEC_IS_SERVICE_TRUSTED(p, service) \
    (((((UINT32*)(p))[(((UINT32)(service)) / BTM_SEC_ARRAY_BITS)]) & \
      (UINT32)(((UINT32)1 << (((UINT32)(service)) % BTM_SEC_ARRAY_BITS)))) \
         ? TRUE \
         : FALSE)

#define BTM_SEC_COPY_TRUSTED_DEVICE(p_src, p_dst) \
    do { \
        register int trst; \
        for (trst = 0; trst < BTM_SEC_SERVICE_ARRAY_SIZE; ++trst) \
            ((UINT32*)(p_dst))[trst] = ((UINT32*)(p_src))[trst]; \
    } while (FALSE)

#define BTM_COD_MAJOR_CLASS_MASK 0x1f

#define BTM_INVALID_SCO_INDEX 0xffff
#define BTM_INVALID_HCI_HANDLE 0xffff
#define BTM_INVALID_SCO_DISC_REASON 0xffff
#define BTM_IGNORE_SCO_PKT_TYPE 0
#define BTM_64KBITS_RATE 0x00001f40

enum {
    BTM_SCO_PKT_TYPES_MASK_HV1 = HCI_ESCO_PKT_TYPES_MASK_HV1,
    BTM_SCO_PKT_TYPES_MASK_HV2 = HCI_ESCO_PKT_TYPES_MASK_HV2,
    BTM_SCO_PKT_TYPES_MASK_HV3 = HCI_ESCO_PKT_TYPES_MASK_HV3,
    BTM_SCO_PKT_TYPES_MASK_EV3 = HCI_ESCO_PKT_TYPES_MASK_EV3,
    BTM_SCO_PKT_TYPES_MASK_EV4 = HCI_ESCO_PKT_TYPES_MASK_EV4,
    BTM_SCO_PKT_TYPES_MASK_EV5 = HCI_ESCO_PKT_TYPES_MASK_EV5,
    BTM_SCO_LINK_ONLY_MASK = BTM_SCO_PKT_TYPES_MASK_HV1 | BTM_SCO_PKT_TYPES_MASK_HV2 | BTM_SCO_PKT_TYPES_MASK_HV3
};

enum {
    BTM_LINK_TYPE_SCO = HCI_LINK_TYPE_SCO,
    BTM_LINK_TYPE_ESCO = HCI_LINK_TYPE_ESCO
};

enum {
    BTM_ESCO_RETRANS_POWER = 1
};

enum {
    BTM_ESCO_CHG_EVT = 1,
    BTM_ESCO_CONN_REQ_EVT
};

enum {
    BTM_COD_MAJOR_PERIPHERAL = 5
};

enum {
    BTM_COD_MINOR_KEYBOARD = 1 << 6
};

typedef UINT8 tBTM_STATUS;

enum {
    BTM_SUCCESS,
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
    BTM_NOT_AUTHORIZED
};

typedef UINT8 tBTM_DEV_STATUS;
typedef UINT8 tBTM_BL_EVENT;

enum {
    BTM_DEV_STATUS_UP,
    BTM_DEV_STATUS_DOWN,
    BTM_DEV_STATUS_CMD_TOUT
};

enum {
    BTM_INQ_RESULT_STANDARD,
    BTM_INQ_RESULT_WITH_RSSI
};

enum {
    BTM_ROLE_MASTER = 0,
    BTM_ROLE_SLAVE = 1,
    BTM_ROLE_UNDEFINED = 255
};

enum {
    BTM_CB_EVT_RETURN_LINK_KEYS = 1
};

typedef UINT16 tBTM_DISC_MODE;

enum {
    BTM_NON_DISCOVERABLE,
    BTM_LIMITED_DISCOVERABLE,
    BTM_GENERAL_DISCOVERABLE
};

typedef UINT16 tBTM_CONN_MODE;

enum {
    BTM_NON_CONNECTABLE,
    BTM_CONNECTABLE
};

typedef UINT8 tBTM_INQ_TYPE;
typedef UINT8 tBTM_INQ_ACTIVE_STATUS;
typedef UINT16 tBTM_INQ_SCAN_TYPE;
typedef UINT8 tBTM_INQ_FILT_COND_TYPE;
typedef UINT8 tBTM_ROLE;
typedef UINT8 tBTM_SCO_TYPE;
typedef UINT8 tBTM_ESCO_EVT;
typedef UINT8 tBTM_PM_STATUS;
enum
{
	BTM_PM_STS_ACTIVE	= HCI_MODE_ACTIVE,
	BTM_PM_STS_HOLD		= HCI_MODE_HOLD,
	BTM_PM_STS_SNIFF	= HCI_MODE_SNIFF,
	BTM_PM_STS_PARK		= HCI_MODE_PARK,
	BTM_PM_STS_SSR		= 4,
	BTM_PM_STS_PENDING	= 4,
	BTM_PM_STS_ERROR	= 5,
};

typedef UINT8 tBTM_PM_MODE;
enum
{
	BTM_PM_REG_SET		= 1 << 0,
	BTM_PM_REG_NOTIF	= 1 << 1,
	BTM_PM_DEREG		= 1 << 2,
};

enum
{
	BTM_PM_MD_ACTIVE	= BTM_PM_STS_ACTIVE,
	BTM_PM_MD_HOLD		= BTM_PM_STS_HOLD,
	BTM_PM_MD_SNIFF		= BTM_PM_STS_SNIFF,
	BTM_PM_MD_PARK		= BTM_PM_STS_PARK,
	BTM_PM_MD_FORCE		= 0x10
};

typedef UINT8 tBTM_AFH_CHANNEL;

enum {
    BTM_SEC_SERVICE_SDP_SERVER = 0,
    BTM_SEC_SERVICE_SERIAL_PORT = 1,
    BTM_SEC_SERVICE_LAN_ACCESS = 2,
    BTM_SEC_SERVICE_DUN = 3,
    BTM_SEC_SERVICE_IRMC_SYNC = 4,
    BTM_SEC_SERVICE_OBEX = 6,
    BTM_SEC_SERVICE_OBEX_FTP = 7,
    BTM_SEC_SERVICE_CORDLESS = 9,
    BTM_SEC_SERVICE_INTERCOM = 10,
    BTM_SEC_SERVICE_FAX = 11,
    BTM_SEC_SERVICE_HEADSET_AG = 12,
    BTM_SEC_SERVICE_BPP_JOB = 22,
    BTM_SEC_SERVICE_BNEP_PANU = 25,
    BTM_SEC_SERVICE_BNEP_GN = 26,
    BTM_SEC_SERVICE_BNEP_NAP = 27,
    BTM_SEC_SERVICE_AG_HANDSFREE = 29,
    BTM_SEC_SERVICE_HID_SEC_CTRL = 32,
    BTM_SEC_SERVICE_HID_NOSEC_CTRL = 33,
    BTM_SEC_SERVICE_HID_INTR = 34,
    BTM_SEC_SERVICE_BIP = 35,
    BTM_SEC_SERVICE_AVDTP = 37,
    BTM_SEC_SERVICE_AVCTP = 39,
    BTM_SEC_SERVICE_SAP = 40,
    BTM_SEC_SERVICE_PBAP = 44
};

enum {
    BTM_SEC_MODE_NONE = 1,
    BTM_SEC_MODE_SERVICE = 2,
    BTM_SEC_MODE_LINK = 3
};

enum {
    BTM_SEC_NONE = 0,
    BTM_SEC_IN_AUTHORIZE = 1 << 0,
    BTM_SEC_IN_AUTHENTICATE = 1 << 1,
    BTM_SEC_IN_ENCRYPT = 1 << 2,
    BTM_SEC_OUT_AUTHORIZE = 1 << 3,
    BTM_SEC_OUT_AUTHENTICATE = 1 << 4,
    BTM_SEC_OUT_ENCRYPT = 1 << 5
};

enum {
    BTM_SEC_FLAG_AUTHORIZED = 1 << 0,
    BTM_SEC_FLAG_AUTHENTICATED = 1 << 1,
    BTM_SEC_FLAG_ENCRYPTED = 1 << 2,
    BTM_SEC_FLAG_LKEY_KNOWN = 1 << 4
};

enum {
    BTM_SEC_PROTO_RFCOMM = 3,
    BTM_SEC_PROTO_HID = 6
};

#define BTM_PM_SET_ONLY_ID 0x80

enum {
    BTM_ACL_MODE_NORMAL = 0,
    BTM_ACL_MODE_HOLD = 1,
    BTM_ACL_MODE_SNIFF = 2,
    BTM_ACL_MODE_PARK = 3
};

typedef UINT8 tBTM_BD_NAME[BTM_MAX_REM_BD_NAME_LEN + 1];

typedef void tBTM_DEV_STATUS_CB(tBTM_DEV_STATUS status);
typedef void tBTM_VS_EVT_CB(UINT8 len, UINT8* p);
typedef void tBTM_CMPL_CB(void* p1);
typedef UINT8 tBTM_FILTER_CB(BD_ADDR bd_addr, DEV_CLASS dc);
typedef void tBTM_INQ_DB_CHANGE_CB(void* p1, BOOLEAN is_new);
typedef void tBTM_INQ_RESULTS_CB(void* p1);
typedef void tBTM_ACL_DB_CHANGE_CB(BD_ADDR p_bda, DEV_CLASS p_dc, BD_NAME p_bdn, BD_FEATURES features, BOOLEAN is_new);
typedef void tBTM_SCO_CB(UINT16 sco_inx);
typedef UINT8 tBTM_AUTHORIZE_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name, UINT8* service_name,
                                      UINT8 service_id, BOOLEAN is_originator);
typedef UINT8 tBTM_PIN_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name);
typedef UINT8 tBTM_LINK_KEY_REQ_CALLBACK(BD_ADDR bd_addr, LINK_KEY key);
typedef UINT8 tBTM_LINK_KEY_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name, UINT8* key, UINT8 key_type);
typedef void tBTM_RMT_NAME_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dc, tBTM_BD_NAME bd_name);
typedef UINT8 tBTM_AUTH_COMPLETE_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name, int result);
typedef UINT8 tBTM_ABORT_CALLBACK(BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name);
typedef void tBTM_MKEY_CALLBACK(BD_ADDR bd_addr, UINT8 status, UINT8 key_flag);
typedef void tBTM_SEC_CBACK(BD_ADDR bd_addr, void* p_ref_data, tBTM_STATUS result);
typedef void tBTM_PM_STATUS_CBACK(BD_ADDR p_bda, tBTM_PM_STATUS status, UINT16 value, tHCI_STATUS hci_status);

typedef struct {
    UINT8 hci_version;
    UINT16 hci_revision;
    UINT8 lmp_version;
    UINT16 manufacturer;
    UINT16 lmp_subversion;
} tBTM_VERSION_INFO;

typedef struct {
    DEV_CLASS dev_class;
    DEV_CLASS dev_class_mask;
} tBTM_COD_COND;

typedef union {
    BD_ADDR bdaddr_cond;
    tBTM_COD_COND cod_cond;
} tBTM_INQ_FILT_COND;

typedef struct {
    tBTM_INQ_TYPE mode;
    UINT8 duration;
    UINT8 max_resps;
    tBTM_INQ_FILT_COND_TYPE filter_cond_type;
    tBTM_INQ_FILT_COND filter_cond;
} tBTM_INQ_PARMS;

typedef struct {
    UINT16 clock_offset;
    BD_ADDR remote_bd_addr;
    DEV_CLASS dev_class;
    UINT8 page_scan_rep_mode;
    UINT8 page_scan_per_mode;
    UINT8 page_scan_mode;
    INT8 rssi;
} tBTM_INQ_RESULTS;

typedef struct {
    tBTM_INQ_RESULTS results;
    BOOLEAN appl_knows_rem_name;
} tBTM_INQ_INFO;

typedef struct {
    tBTM_STATUS status;
    UINT8 num_resp;
} tBTM_INQUIRY_CMPL;

typedef struct {
    UINT16 status;
    UINT16 length;
    BD_NAME remote_bd_name;
} tBTM_REMOTE_DEV_NAME;

typedef struct {
    tHCI_STATUS hci_status;
    tBTM_ROLE role;
    BD_ADDR remote_bd_addr;
} tBTM_ROLE_SWITCH_CMPL;

typedef struct {
    UINT32 tx_bw;
    UINT32 rx_bw;
    UINT16 max_latency;
    UINT16 voice_contfmt;
    UINT16 packet_types;
    UINT8 retrans_effort;
} tBTM_ESCO_PARAMS;

typedef struct {
    UINT16 max_latency;
    UINT16 packet_types;
    UINT8 retrans_effort;
} tBTM_CHG_ESCO_PARAMS;

typedef struct {
    UINT16 rx_pkt_len;
    UINT16 tx_pkt_len;
    BD_ADDR bd_addr;
    UINT8 link_type;
    UINT8 tx_interval;
    UINT8 retrans_window;
    UINT8 air_mode;
} tBTM_ESCO_DATA;

typedef struct {
    UINT16 sco_inx;
    UINT16 rx_pkt_len;
    UINT16 tx_pkt_len;
    BD_ADDR bd_addr;
    UINT8 hci_status;
    UINT8 tx_interval;
    UINT8 retrans_window;
} tBTM_CHG_ESCO_EVT_DATA;

typedef struct {
    UINT16 sco_inx;
    BD_ADDR bd_addr;
    DEV_CLASS dev_class;
    tBTM_SCO_TYPE link_type;
} tBTM_ESCO_CONN_REQ_EVT_DATA;

typedef union {
    tBTM_CHG_ESCO_EVT_DATA chg_evt;
    tBTM_ESCO_CONN_REQ_EVT_DATA conn_evt;
} tBTM_ESCO_EVT_DATA;

typedef void tBTM_ESCO_CBACK(tBTM_ESCO_EVT event, tBTM_ESCO_EVT_DATA* p_data);

typedef struct {
    tBTM_AUTHORIZE_CALLBACK* p_authorize_callback;
    tBTM_PIN_CALLBACK* p_pin_callback;
    tBTM_LINK_KEY_CALLBACK* p_link_key_callback;
    tBTM_LINK_KEY_REQ_CALLBACK* p_link_key_req_callback;
    tBTM_AUTH_COMPLETE_CALLBACK* p_auth_complete_callback;
    tBTM_ABORT_CALLBACK* p_abort_callback;
} tBTM_APPL_INFO;

typedef struct {
    UINT16 max;
    UINT16 min;
    UINT16 attempt;
    UINT16 timeout;
    tBTM_PM_MODE mode;
} tBTM_PM_PWR_MD;

typedef struct {
    UINT8 event;
    UINT8 num_keys;
} tBTM_RETURN_LINK_KEYS_EVT;

typedef struct
{
	tBTM_STATUS	status;
	UINT8		hci_status;
	INT8		rssi;
	BD_ADDR		rem_bda;
} tBTM_RSSI_RESULTS;

typedef struct
{
	tBTM_STATUS	status;
	UINT8		hci_status;
	UINT8		link_quality;
	BD_ADDR		rem_bda;
} tBTM_LINK_QUALITY_RESULTS;

BOOLEAN BTM_IsDeviceUp(void);
void BTM_DeviceReset(tBTM_CMPL_CB* p_cb);

void BTM_SetDefaultLinkPolicy(UINT16 settings);
void BTM_SetDefaultLinkSuperTout(UINT16 timeout);
BOOLEAN BTM_IsAclConnectionUp(BD_ADDR remote_bda);
UINT16 BTM_GetNumAclLinks(void);
tBTM_STATUS BTM_AclRegisterForChanges(tBTM_ACL_DB_CHANGE_CB* p_cb);
tBTM_STATUS BTM_ReadLocalDeviceAddr(tBTM_CMPL_CB* p_cb);
char* BTM_SecReadDevName(BD_ADDR bd_addr);
tBTM_STATUS BTM_SetLocalDeviceName(char* p_name);
tBTM_STATUS BTM_SetDeviceClass(DEV_CLASS dev_class);
tBTM_STATUS BTM_WritePageTimeout(UINT16 timeout);
tBTM_STATUS BTM_SetDiscoverability(UINT16 inq_mode, UINT16 window, UINT16 interval);
tBTM_STATUS BTM_SetConnectability(UINT16 page_mode, UINT16 window, UINT16 interval);
UINT16 BTM_IsInquiryActive(void);
tBTM_STATUS BTM_CancelInquiry(void);
tBTM_STATUS BTM_StartInquiry(tBTM_INQ_PARMS* p_inqparms, tBTM_INQ_RESULTS_CB* p_results_cb, tBTM_CMPL_CB* p_cmpl_cb);
tBTM_STATUS BTM_ReadRemoteDeviceName(BD_ADDR remote_bda, tBTM_CMPL_CB* p_cb);
tBTM_STATUS BTM_SwitchRole(BD_ADDR remote_bd_addr, UINT8 new_role, tBTM_CMPL_CB* p_cb);
UINT8* BTM_ReadLocalFeatures(void);
tBTM_INQ_INFO* BTM_InqDbRead(BD_ADDR p_bda);
tBTM_STATUS BTM_CancelRemoteDeviceName(void);
tBTM_INQ_INFO* BTM_InqDbFirst(void);
tBTM_INQ_INFO* BTM_InqDbNext(tBTM_INQ_INFO* p_cur);
tBTM_STATUS BTM_ClearInqDb(BD_ADDR p_bda);
BOOLEAN BTM_SecRegister(tBTM_APPL_INFO* p_cb_info);
BOOLEAN BTM_SecAddRmtNameNotifyCallback(tBTM_RMT_NAME_CALLBACK* p_callback);
BOOLEAN BTM_SecDeleteRmtNameNotifyCallback(tBTM_RMT_NAME_CALLBACK* p_callback);
BOOLEAN BTM_SecAddLinkKeyNotifyCallback(tBTM_LINK_KEY_CALLBACK* p_callback);
BOOLEAN BTM_SecDeleteLinkKeyNotifyCallback(tBTM_LINK_KEY_CALLBACK* p_callback);
void BTM_SecSetConnectFilterCallback(tBTM_FILTER_CB* p_callback);
void BTM_SetSecurityMode(UINT8 security_mode);
void BTM_SetPinType(UINT8 pin_type, PIN_CODE pin_code, UINT8 pin_code_len);
void BTM_SetPairableMode(BOOLEAN allow_pairing, BOOLEAN connect_only_paired);
UINT8 BTM_SecClrService(UINT8 service_id);
tBTM_STATUS BTM_SecUseMasterLinkKey(BOOLEAN use_master_key);
void BTM_SetMasterKeyCompCback(tBTM_MKEY_CALLBACK* mkey_cback);
tBTM_STATUS BTM_SecGetDeviceLinkKey(BD_ADDR bd_addr, LINK_KEY link_key);
tBTM_STATUS BTM_SetEncryption(BD_ADDR bd_addr, tBTM_SEC_CBACK* p_callback, void* p_ref_data);
tBTM_STATUS BTM_DeleteStoredLinkKey(BD_ADDR bd_addr, tBTM_CMPL_CB* p_cb);
void BTM_PINCodeReply(BD_ADDR bd_addr, UINT8 res, UINT8 pin_len, UINT8* p_pin, UINT32 trusted_mask[]);
void BTM_DeviceAuthorized(BD_ADDR bd_addr, UINT8 res, UINT32 trusted_mask[]);
tBTM_STATUS BTM_SecBond(BD_ADDR bd_addr, UINT8 pin_len, UINT8* p_pin, UINT32 trusted_mask[]);
tBTM_STATUS BTM_SetPowerMode(UINT8 pm_id, BD_ADDR remote_bda, tBTM_PM_PWR_MD* p_mode);
tBTM_STATUS BTM_ReadPowerMode(BD_ADDR remote_bda, tBTM_PM_MODE* p_mode);
BOOLEAN BTM_SetSecurityLevel(BOOLEAN is_originator, char* p_name, UINT8 service_id, UINT8 sec_level, UINT16 psm,
                             UINT32 mx_proto_id, UINT32 mx_chan_id);

tBTM_STATUS BTM_SwitchRole(BD_ADDR remote_bd_addr, UINT8 new_role,
                           tBTM_CMPL_CB *p_cb);
tBTM_STATUS BTM_ReadRSSI(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb);
tBTM_STATUS BTM_ReadLinkQuality(BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb);
void BTM_SendHciReset(tBTM_CMPL_CB *p_cb);
tBTM_INQ_INFO *BTM_InqDbRead(BD_ADDR p_bda);
UINT16 BTM_GetHCIConnHandle(BD_ADDR remote_bda);
tBTM_STATUS BTM_CreateSco(BD_ADDR remote_bda, BOOLEAN is_orig, UINT16 pkt_types, UINT16* p_sco_inx, tBTM_SCO_CB* p_conn_cb,
                          tBTM_SCO_CB* p_disc_cb);
tBTM_STATUS BTM_RemoveSco(UINT16 sco_inx);
tBTM_STATUS BTM_SetScoPacketTypes(UINT16 sco_inx, UINT16 pkt_types);
UINT16 BTM_ReadScoPacketTypes(UINT16 sco_inx);
UINT16 BTM_ReadScoDiscReason(void);
UINT16 BTM_ReadDeviceScoPacketTypes(void);
UINT16 BTM_ReadScoHandle(UINT16 sco_inx);
BD_ADDR_PTR BTM_ReadScoBdAddr(UINT16 sco_inx);
tBTM_STATUS BTM_SetEScoMode(tBTM_SCO_TYPE sco_mode, tBTM_ESCO_PARAMS* p_parms);
tBTM_STATUS BTM_RegForEScoEvts(UINT16 sco_inx, tBTM_ESCO_CBACK* p_esco_cback);
tBTM_STATUS BTM_ReadEScoLinkParms(UINT16 sco_inx, tBTM_ESCO_DATA* p_parms);
tBTM_STATUS BTM_ChangeEScoLinkParms(UINT16 sco_inx, tBTM_CHG_ESCO_PARAMS* p_parms);
void BTM_EScoConnRsp(UINT16 sco_inx, UINT8 hci_status, tBTM_ESCO_PARAMS* p_parms);
tBTM_STATUS BTM_ReadLocalVersion(tBTM_VERSION_INFO *p_vers);
tBTM_STATUS BTM_PmRegister(UINT8 mask, UINT8 *p_pm_id,
                           tBTM_PM_STATUS_CBACK *p_cb);
BOOLEAN BTM_SecAddDevice(BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name,
                         BD_FEATURES features, UINT32 *trusted_mask,
                         LINK_KEY link_key);
BOOLEAN BTM_SecDeleteDevice(BD_ADDR bd_addr);
#endif
