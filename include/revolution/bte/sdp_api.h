#ifndef RVL_SDK_BTE_SDP_API_H
#define RVL_SDK_BTE_SDP_API_H

#include <revolution/bte/bt_target.h>
#include <revolution/bte/bt_types.h>

typedef UINT16 tSDP_STATUS;

#define SDP_SUCCESS 0
#define SDP_INVALID_SERV_REC_HDL 2
#define SDP_INVALID_REQ_SYNTAX 3
#define SDP_INVALID_PDU_SIZE 4
#define SDP_INVALID_CONT_STATE 5
#define SDP_ILLEGAL_PARAMETER 11
#define SDP_NO_RECS_MATCH 0xFFF0
#define SDP_CONN_FAILED 0xFFF1
#define SDP_CFG_FAILED 0xFFF2
#define SDP_GENERIC_ERROR 0xFFF3
#define SDP_DB_FULL 0xFFF4
#define SDP_SECURITY_ERR 0xFFF6
#define SDP_CONN_REJECTED 0xFFF7

#define SDP_PSM 0x0001

#define SDP_DISC_ATTR_LEN_MASK 0x0FFF
#define SDP_DISC_ATTR_TYPE(len_type) ((len_type) >> 12)
#define SDP_DISC_ATTR_LEN(len_type) ((len_type) & SDP_DISC_ATTR_LEN_MASK)

typedef void tSDP_DISC_CMPL_CB(UINT16 result);

typedef tBT_UUID tSDP_UUID;

typedef struct {
    union {
        UINT8 u8;
        UINT16 u16;
        UINT32 u32;
        UINT8 array[4];
        struct t_sdp_disc_attr* p_sub_attr;
    } v;
} tSDP_DISC_ATVAL;

typedef struct t_sdp_disc_attr {
    struct t_sdp_disc_attr* p_next_attr;
    UINT16 attr_id;
    UINT16 attr_len_type;
    tSDP_DISC_ATVAL attr_value;
} tSDP_DISC_ATTR;

typedef struct t_sdp_disc_rec {
    tSDP_DISC_ATTR* p_first_attr;
    struct t_sdp_disc_rec* p_next_rec;
    UINT32 time_read;
    BD_ADDR remote_bd_addr;
} tSDP_DISC_REC;

typedef struct {
    UINT32 mem_size;
    UINT32 mem_free;
    tSDP_DISC_REC* p_first_rec;
    UINT16 num_uuid_filters;
    tSDP_UUID uuid_filters[SDP_MAX_UUID_FILTERS];
    UINT16 num_attr_filters;
    UINT16 attr_filters[SDP_MAX_ATTR_FILTERS];
    UINT8* p_free_mem;
} tSDP_DISCOVERY_DB;

BOOLEAN SDP_InitDiscoveryDb(tSDP_DISCOVERY_DB* p_db, UINT32 len, UINT16 num_uuid, tSDP_UUID* p_uuid_list, UINT16 num_attr,
                            UINT16* p_attr_list);
BOOLEAN SDP_ServiceSearchRequest(UINT8* p_bd_addr, tSDP_DISCOVERY_DB* p_db, tSDP_DISC_CMPL_CB* p_cb);

typedef struct t_sdp_di_record {
    UINT16 vendor;
    UINT16 vendor_id_source;
    UINT16 product;
    UINT16 version;
    BOOLEAN primary_record;
    char client_executable_url[SDP_MAX_ATTR_LEN];
    char service_description[SDP_MAX_ATTR_LEN];
    char documentation_url[SDP_MAX_ATTR_LEN];
} tSDP_DI_RECORD;

typedef struct t_sdp_di_get_record {
    UINT16 spec_id;
    tSDP_DI_RECORD rec;
} tSDP_DI_GET_RECORD;

tSDP_STATUS SDP_SetLocalDiRecord(tSDP_DI_RECORD* p_device_info, UINT32* p_handle);
tSDP_STATUS SDP_GetLocalDiRecord(tSDP_DI_GET_RECORD* p_device_info, UINT32* p_handle);
BOOLEAN SDP_AddUuidSequence(UINT32 handle, UINT16 attr_id, UINT16 num_uuids, UINT16* p_uuids);
BOOLEAN SDP_AddAttribute(UINT32 handle, UINT16 attr_id, UINT8 attr_type, UINT32 attr_len, UINT8* p_val);
tSDP_DISC_ATTR* SDP_FindAttributeInRec(tSDP_DISC_REC* p_rec, UINT16 attr_id);
tSDP_DISC_REC* SDP_FindServiceInDb(tSDP_DISCOVERY_DB* p_db, UINT16 service_uuid, tSDP_DISC_REC* p_start_rec);
tSDP_DISC_REC* SDP_FindServiceUUIDInDb(tSDP_DISCOVERY_DB* p_db, tBT_UUID* p_uuid, tSDP_DISC_REC* p_start_rec);

#endif
