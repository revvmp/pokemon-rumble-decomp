#ifndef RVL_SDK_BTE_SDP_INT_H
#define RVL_SDK_BTE_SDP_INT_H

#include <revolution/bte/bt_target.h>
#include <revolution/bte/bt_types.h>
#include <revolution/bte/gki.h>
#include <revolution/bte/l2c_api.h>
#include <revolution/bte/sdp_api.h>

#define SDP_CONTINUATION_LEN 2

#define SDP_INACT_TIMEOUT 30

#define MAX_UUIDS_PER_SEQ 16
#define MAX_ATTR_PER_SEQ 16

enum {
    SDP_PDU_ERROR_RESPONSE = 1,
    SDP_PDU_SERVICE_SEARCH_REQ = 2,
    SDP_PDU_SERVICE_SEARCH_RSP = 3,
    SDP_PDU_SERVICE_ATTR_REQ = 4,
    SDP_PDU_SERVICE_ATTR_RSP = 5,
    SDP_PDU_SERVICE_SEARCH_ATTR_REQ = 6,
    SDP_PDU_SERVICE_SEARCH_ATTR_RSP = 7
};

enum {
    SDP_STATE_IDLE,
    SDP_STATE_CONN_SETUP,
    SDP_STATE_CFG_SETUP,
    SDP_STATE_CONNECTED
};

enum {
    SDP_FLAGS_IS_ORIG = 0x01,
    SDP_FLAGS_THEIR_CFG_DONE = 0x02,
    SDP_FLAGS_OUR_CFG_DONE = 0x04
};

typedef struct {
    UINT16 len;
    UINT8 value[MAX_UUID_SIZE];
} tUID_ENT;

typedef struct {
    UINT16 num_uids;
    tUID_ENT uuid_entry[MAX_UUIDS_PER_SEQ];
} tSDP_UUID_SEQ;

typedef struct {
    UINT16 start;
    UINT16 end;
} tATT_ENT;

typedef struct {
    UINT16 num_attr;
    tATT_ENT attr_entry[MAX_ATTR_PER_SEQ];
} tSDP_ATTR_SEQ;

typedef struct {
    UINT32 len;
    UINT8* value_ptr;
    UINT16 id;
    UINT8 type;
} tSDP_ATTRIBUTE;

typedef struct {
    UINT32 record_handle;
    UINT32 free_pad_ptr;
    UINT16 num_attributes;
    tSDP_ATTRIBUTE attribute[SDP_MAX_REC_ATTR];
    UINT8 attr_pad[SDP_MAX_PAD_LEN];
} tSDP_RECORD;

typedef struct {
    UINT32 di_primary_handle;
    BOOLEAN brcm_di_registered;
    UINT16 num_records;
    tSDP_RECORD record[SDP_MAX_RECORDS];
} tSDP_DB;

typedef struct {
    UINT8 con_state;
    UINT8 con_flags;
    BD_ADDR device_address;
    TIMER_LIST_ENT timer_entry;
    UINT16 rem_mtu_size;
    UINT16 connection_id;
    UINT16 list_len;
    UINT8 rsp_list[1000];
    tSDP_DISCOVERY_DB* p_db;
    tSDP_DISC_CMPL_CB* p_cb;
    UINT32 handles[SDP_MAX_DISC_SERVER_RECS];
    UINT16 num_handles;
    UINT16 cur_handle;
    UINT16 transaction_id;
    UINT16 disconnect_reason;
    UINT8 disc_state;
    BOOLEAN is_attr_search;
    UINT16 cont_offset;
} tCONN_CB;

typedef struct {
    tL2CAP_CFG_INFO l2cap_my_cfg;
    tCONN_CB ccb[SDP_MAX_CONNECTIONS];
    tSDP_DB server_db;
    tL2CAP_APPL_INFO reg_info;
    UINT16 max_attr_list_size;
    UINT16 max_recs_per_search;
    UINT8 trace_level;
} tSDP_CB;

extern tSDP_CB sdp_cb;

tSDP_RECORD* sdp_db_service_search(tSDP_RECORD* p_rec, tSDP_UUID_SEQ* p_seq);
tSDP_RECORD* sdp_db_find_record(UINT32 handle);
tSDP_ATTRIBUTE* sdp_db_find_attr_in_rec(tSDP_RECORD* p_rec, UINT16 start_attr, UINT16 end_attr);

void sdp_disc_connected(tCONN_CB* p_ccb);
void sdp_disc_server_rsp(tCONN_CB* p_ccb, BT_HDR* p_msg);

void sdp_init(void);
tCONN_CB* sdp_conn_originate(UINT8* p_bd_addr);
void sdp_disconnect(tCONN_CB* p_ccb, UINT16 reason);
void sdp_conn_timeout(tCONN_CB* p_ccb);

void sdp_server_handle_client_req(tCONN_CB* p_ccb, BT_HDR* p_msg);

tCONN_CB* sdpu_find_ccb_by_cid(UINT16 cid);
tCONN_CB* sdpu_allocate_ccb(void);
void sdpu_release_ccb(tCONN_CB* p_ccb);
UINT8* sdpu_build_attrib_seq(UINT8* p_out, UINT16* p_attr, UINT16 num_attrs);
UINT8* sdpu_build_attrib_entry(UINT8* p_out, tSDP_ATTRIBUTE* p_attr);
void sdpu_build_n_send_error(tCONN_CB* p_ccb, UINT16 trans_num, UINT16 error_code, char* p_error_text);
UINT8* sdpu_extract_uid_seq(UINT8* p, UINT16 param_len, tSDP_UUID_SEQ* p_seq);
UINT8* sdpu_extract_attr_seq(UINT8* p, UINT16 param_len, tSDP_ATTR_SEQ* p_seq);
UINT8* sdpu_get_len_from_type(UINT8* p, UINT8 type, UINT32* p_len);
BOOLEAN sdpu_is_base_uuid(UINT8* p_uuid);
BOOLEAN sdpu_compare_uuid_arrays(UINT8* p_uuid1, UINT32 len1, UINT8* p_uuid2, UINT16 len2);
BOOLEAN sdpu_compare_uuid_with_attr(tBT_UUID* p_btuuid, tSDP_DISC_ATTR* p_attr);
void sdpu_sort_attr_list(UINT16 num_attr, tSDP_DISCOVERY_DB* p_db);

#endif
