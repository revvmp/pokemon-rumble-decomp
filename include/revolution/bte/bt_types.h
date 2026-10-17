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

enum {
    BT_EVT_TO_BTU_HCI_EVT = 0x1000,
    BT_EVT_TO_BTU_HCI_ACL = 0x1100,
    BT_EVT_TO_BTU_HCI_SCO = 0x1200,
    BT_EVT_TO_BTU_HCI_CMD = 0x1600,
    BT_EVT_TO_BTU_L2C_SEG_XMIT = 0x1900,
    BT_EVT_MASK = 0xFF00
};
#define BT_EVT_TO_LM_HCI_CMD 0x2000
#define BT_EVT_TO_LM_HCI_ACL 0x2100

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
    NO_TRAFFIC = 0,
    BEST_EFFORT = 1,
    GUARANTEED = 2
};

enum {
    BT_PSM_RFCOMM = 3,
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

#define UINT8_TO_BE_STREAM(p, u8)                                                                                      \
    do {                                                                                                               \
        *(p)++ = (UINT8)(u8);                                                                                          \
    } while (FALSE)

#define UINT16_TO_BE_STREAM(p, u16)                                                                                    \
    do {                                                                                                               \
        *(p)++ = (UINT8)((u16) >> 8);                                                                                  \
        *(p)++ = (UINT8)(u16);                                                                                         \
    } while (FALSE)

#define UINT32_TO_BE_STREAM(p, u32)                                                                                    \
    do {                                                                                                               \
        *(p)++ = (UINT8)((u32) >> 24);                                                                                 \
        *(p)++ = (UINT8)((u32) >> 16);                                                                                 \
        *(p)++ = (UINT8)((u32) >> 8);                                                                                  \
        *(p)++ = (UINT8)(u32);                                                                                         \
    } while (FALSE)

#define ARRAY_TO_BE_STREAM(p, a, len)                                                                                  \
    do {                                                                                                               \
        register int ijk;                                                                                              \
        for (ijk = 0; ijk < len; ijk++)                                                                                \
            *(p)++ = (UINT8)(a)[ijk];                                                                                  \
    } while (FALSE)

#define BE_STREAM_TO_UINT8(p, u8)                                                                                      \
    do {                                                                                                               \
        *(u8) = (UINT8)(*(p));                                                                                         \
        (p) += 1;                                                                                                      \
    } while (FALSE)

#define BE_STREAM_TO_UINT16(p, u16)                                                                                    \
    do {                                                                                                               \
        *(u16) = (UINT16)(((UINT16)(*(p)) << 8) + (UINT16)(*((p) + 1)));                                               \
        (p) += 2;                                                                                                      \
    } while (FALSE)

#define BE_STREAM_TO_UINT32(p, u32)                                                                                    \
    do {                                                                                                               \
        *(u32) = ((UINT32)(*((p) + 3)) + ((UINT32)(*((p) + 2)) << 8) + ((UINT32)(*((p) + 1)) << 16) +                  \
                  ((UINT32)(*(p)) << 24));                                                                             \
        (p) += 4;                                                                                                      \
    } while (FALSE)

#define BE_STREAM_TO_ARRAY(p, a, len)                                                                                  \
    do {                                                                                                               \
        register int ijk;                                                                                              \
        for (ijk = 0; ijk < len; ijk++)                                                                                \
            ((UINT8*)(a))[ijk] = *(p)++;                                                                               \
    } while (FALSE)

#define STREAM_TO_UINT8(p, u8)                                                                                         \
    do {                                                                                                               \
        *(u8) = (UINT8)(*(p));                                                                                         \
        (p) += 1;                                                                                                      \
    } while (FALSE)

#define STREAM_TO_UINT16(p, u16)                                                                                       \
    do {                                                                                                               \
        *(u16) = (UINT16)((UINT16)(*(p)) + ((UINT16)(*((p) + 1)) << 8));                                               \
        (p) += 2;                                                                                                      \
    } while (FALSE)

#define STREAM_TO_UINT32(p, u32)                                                                                       \
    do {                                                                                                               \
        *(u32) = ((UINT32)(*(p)) + ((UINT32)(*((p) + 1)) << 8) + ((UINT32)(*((p) + 2)) << 16) +                        \
                  ((UINT32)(*((p) + 3)) << 24));                                                                       \
        (p) += 4;                                                                                                      \
    } while (FALSE)

#define REVERSED_STREAM_TO_ARRAY(p, a, len)                                                                            \
    do {                                                                                                               \
        register int ijk;                                                                                              \
        register UINT8* _pa = (UINT8*)(a) + (len) - 1;                                                                 \
        for (ijk = 0; ijk < (len); ++ijk)                                                                              \
            *_pa-- = *(p)++;                                                                                           \
    } while (FALSE)

#define STREAM_TO_ARRAY16(p, a) REVERSED_STREAM_TO_ARRAY(p, a, LINK_KEY_LEN)
#define STREAM_TO_BDADDR(p, a) REVERSED_STREAM_TO_ARRAY(p, a, BD_ADDR_LEN)
#define STREAM_TO_DEVCLASS(p, a) REVERSED_STREAM_TO_ARRAY(p, a, DEV_CLASS_LEN)

#define UINT32_TO_BE_FIELD(p, u32)                                                                                     \
    do {                                                                                                               \
        *((UINT8*)(p) + 0) = (UINT8)((u32) >> 24);                                                                     \
        *((UINT8*)(p) + 1) = (UINT8)((u32) >> 16);                                                                     \
        *((UINT8*)(p) + 2) = (UINT8)((u32) >> 8);                                                                      \
        *((UINT8*)(p) + 3) = (UINT8)(u32);                                                                             \
    } while (FALSE)

#endif
