#ifndef RVL_SDK_BTE_BT_TRACE_H
#define RVL_SDK_BTE_BT_TRACE_H

#include <RVL_SDK/bte/data_types.h>

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

void LogMsg_0(UINT32 trace_set_mask, const char* fmt_str);

#define APPL_TRACE_EVENT0(m)                                                                                     \
    {                                                                                                            \
        if (appl_trace_level >= BT_TRACE_LEVEL_EVENT)                                                            \
            LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_EVENT, m);           \
    }

#endif
