#ifndef RVL_SDK_BTE_SDP_API_H
#define RVL_SDK_BTE_SDP_API_H

#include "RVL_SDK/bte/bt_types.h"

enum {
    SDP_SUCCESS = 0,
    SDP_ILLEGAL_PARAMETER = 11
};

#define SDP_MAX_UUID_FILTERS 3
#define SDP_MAX_ATTR_FILTERS 12

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
tSDP_DISC_REC* SDP_FindServiceUUIDInDb(tSDP_DISCOVERY_DB* p_db, tBT_UUID* p_uuid, tSDP_DISC_REC* p_start_rec);
tSDP_DISC_ATTR* SDP_FindAttributeInRec(tSDP_DISC_REC* p_rec, UINT16 attr_id);

#endif
