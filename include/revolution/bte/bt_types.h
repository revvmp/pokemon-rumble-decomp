#ifndef RVL_SDK_BTE_BT_TYPES_H
#define RVL_SDK_BTE_BT_TYPES_H

#include <revolution/bte/data_types.h>

#define BD_ADDR_LEN 6
typedef UINT8 BD_ADDR[BD_ADDR_LEN];
typedef UINT8* BD_ADDR_PTR;

#define LINK_KEY_LEN 16
typedef UINT8 LINK_KEY[LINK_KEY_LEN];

#define DEV_CLASS_LEN 3
typedef UINT8 DEV_CLASS[DEV_CLASS_LEN];

#define BD_NAME_LEN 248
typedef UINT8 BD_NAME[BD_NAME_LEN];

#define PIN_CODE_LEN 16
typedef UINT8 PIN_CODE[PIN_CODE_LEN];

#define LAP_LEN 3
typedef UINT8 LAP[LAP_LEN];

#define BD_FEATURES_LEN 8
typedef UINT8 BD_FEATURES[BD_FEATURES_LEN];

typedef UINT16 tBT_EVT;

typedef struct {
    tBT_EVT event;
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

#define BT_PASS 0

#define UINT8_TO_STREAM(p, u8)                                                                                         \
    do {                                                                                                               \
        *(p)++ = (UINT8)(u8);                                                                                          \
    } while (FALSE)

#define UINT16_TO_STREAM(p, u16)                                                                                       \
    do {                                                                                                               \
        *(p)++ = (UINT8)(u16);                                                                                         \
        *(p)++ = (UINT8)((u16) >> 8);                                                                                  \
    } while (FALSE)

#define UINT32_TO_STREAM(p, u32)                                                                                       \
    do {                                                                                                               \
        *(p)++ = (UINT8)(u32);                                                                                         \
        *(p)++ = (UINT8)((u32) >> 8);                                                                                  \
        *(p)++ = (UINT8)((u32) >> 16);                                                                                 \
        *(p)++ = (UINT8)((u32) >> 24);                                                                                 \
    } while (FALSE)

#define REVERSED_ARRAY_TO_STREAM(p, a, len)                                                                            \
    do {                                                                                                               \
        register int ijk;                                                                                              \
        for (ijk = 0; ijk < (len); ++ijk)                                                                              \
            *(p)++ = (UINT8)(a)[((len) - 1) - ijk];                                                                    \
    } while (FALSE)

#define ARRAY16_TO_STREAM(p, a) REVERSED_ARRAY_TO_STREAM(p, a, 16)
#define BDADDR_TO_STREAM(p, a) REVERSED_ARRAY_TO_STREAM(p, a, BD_ADDR_LEN)
#define DEVCLASS_TO_STREAM(p, a) REVERSED_ARRAY_TO_STREAM(p, a, DEV_CLASS_LEN)
#define LAP_TO_STREAM(p, a) REVERSED_ARRAY_TO_STREAM(p, a, LAP_LEN)

#define ARRAY_TO_STREAM(p, a, len)                                                                                     \
    do {                                                                                                               \
        register int ijk;                                                                                              \
        for (ijk = 0; ijk < (len); ++ijk)                                                                              \
            *(p)++ = (UINT8)(a)[ijk];                                                                                  \
    } while (FALSE)

#endif
