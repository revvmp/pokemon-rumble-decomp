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

#endif
