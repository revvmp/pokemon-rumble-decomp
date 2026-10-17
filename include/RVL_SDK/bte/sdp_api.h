#ifndef RVL_SDK_BTE_SDP_API_H
#define RVL_SDK_BTE_SDP_API_H

#include <RVL_SDK/bte/bt_target.h>
#include <RVL_SDK/bte/bt_types.h>

typedef UINT16 tSDP_STATUS;

enum {
    SDP_SUCCESS = 0,
    SDP_NO_RECS_MATCH = 0xFFF0,
    SDP_CONN_FAILED = 0xFFF1,
    SDP_CFG_FAILED = 0xFFF2,
    SDP_GENERIC_ERROR = 0xFFF3,
    SDP_DB_FULL = 0xFFF4,
    SDP_SECURITY_ERR = 0xFFF6,
    SDP_CONN_REJECTED = 0xFFF7
};

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

tSDP_DISC_ATTR* SDP_FindAttributeInRec(tSDP_DISC_REC* p_rec, UINT16 attr_id);
tSDP_DISC_REC* SDP_FindServiceInDb(tSDP_DISCOVERY_DB* p_db, UINT16 service_uuid, tSDP_DISC_REC* p_start_rec);

#endif
