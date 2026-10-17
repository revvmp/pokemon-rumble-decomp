#ifndef RVL_SDK_BTE_BT_TYPES_H
#define RVL_SDK_BTE_BT_TYPES_H

#include "RVL_SDK/bte/data_types.h"

#define BD_ADDR_LEN 6
typedef UINT8 BD_ADDR[BD_ADDR_LEN];

typedef struct {
    UINT16 event;
    UINT16 len;
    UINT16 offset;
    UINT16 layer_specific;
} BT_HDR;

#define LEN_UUID_16 2
#define LEN_UUID_32 4
#define LEN_UUID_128 16
#define MAX_UUID_SIZE 16

typedef struct {
    UINT16 len;
    union {
        UINT16 uuid16;
        UINT32 uuid32;
        UINT8 uuid128[MAX_UUID_SIZE];
    } uu;
} tBT_UUID;

typedef struct {
    UINT8 qos_flags;
    UINT8 service_type;
    UINT32 token_rate;
    UINT32 token_bucket_size;
    UINT32 peak_bandwidth;
    UINT32 latency;
    UINT32 delay_variation;
} FLOW_SPEC;

enum {
    BT_PSM_HIDC = 17,
    BT_PSM_HIDI = 19
};

#define HID_PSM_CONTROL BT_PSM_HIDC
#define HID_PSM_INTERRUPT BT_PSM_HIDI

#endif
