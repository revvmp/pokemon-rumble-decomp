#ifndef RVL_SDK_BTE_BT_TRACE_H
#define RVL_SDK_BTE_BT_TRACE_H

#include <revolution/bte/data_types.h>

typedef UINT8 tBT_TRACE_LEVEL;

enum {
    BT_TRACE_LEVEL_NONE = 0,
    BT_TRACE_LEVEL_ERROR = 1,
    BT_TRACE_LEVEL_WARNING = 2,
    BT_TRACE_LEVEL_API = 3,
    BT_TRACE_LEVEL_EVENT = 4,
    BT_TRACE_LEVEL_DEBUG = 5
};

#define TRACE_CTRL_GENERAL 0x00000000
#define TRACE_LAYER_NONE 0x00000000
#define TRACE_ORG_APPL 0x00000500
#define TRACE_TYPE_EVENT 0x00000003

#define TRACE_LAYER_HID 0x001E0000
#define TRACE_ORG_STACK 0x00000000
#define TRACE_TYPE_ERROR 0x00000000
#define TRACE_TYPE_WARNING 0x00000001
#define TRACE_TYPE_API 0x00000002
#define TRACE_TYPE_DEBUG 0x00000004

void LogMsg_0(UINT32 trace_set_mask, const char* fmt_str);
void LogMsg_1(UINT32 trace_set_mask, const char* fmt_str, UINT32 p1);

#define APPL_TRACE_EVENT0(m)                                                                                     \
    {                                                                                                            \
        if (appl_trace_level >= BT_TRACE_LEVEL_EVENT)                                                            \
            LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_EVENT, m);           \
    }

#define HID_TRACE_MASK(type) (TRACE_LAYER_HID | TRACE_ORG_STACK | (type))

#define HIDD_TRACE_WARNING0(m)                                                                                         \
    do {                                                                                                               \
        if (hd_cb.trace_level >= BT_TRACE_LEVEL_WARNING)                                                               \
            LogMsg_0(HID_TRACE_MASK(TRACE_TYPE_WARNING), (m));                                                         \
    } while (0)

#define HIDD_TRACE_EVENT0(m)                                                                                           \
    do {                                                                                                               \
        if (hd_cb.trace_level >= BT_TRACE_LEVEL_EVENT)                                                                 \
            LogMsg_0(HID_TRACE_MASK(TRACE_TYPE_EVENT), (m));                                                           \
    } while (0)

#define HIDD_TRACE_DEBUG0(m)                                                                                           \
    do {                                                                                                               \
        if (hd_cb.trace_level >= BT_TRACE_LEVEL_DEBUG)                                                                 \
            LogMsg_0(HID_TRACE_MASK(TRACE_TYPE_DEBUG), (m));                                                           \
    } while (0)

#define HIDH_TRACE_ERROR0(m)                                                                                           \
    do {                                                                                                               \
        if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR)                                                                 \
            LogMsg_0(HID_TRACE_MASK(TRACE_TYPE_ERROR), (m));                                                           \
    } while (0)

#define HIDH_TRACE_ERROR1(m, p1)                                                                                       \
    do {                                                                                                               \
        if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR)                                                                 \
            LogMsg_1(HID_TRACE_MASK(TRACE_TYPE_ERROR), (m), (UINT32)(p1));                                             \
    } while (0)

#endif
