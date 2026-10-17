#include <nw4r/snd/snd_debug.h>

namespace nw4r {
namespace snd {
namespace {

enum DebugWarningBitFlag {
    DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_SEQSOUND = (1 << 0),
    DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_STRMSOUND = (1 << 1),
    DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_WAVESOUND = (1 << 2),
    DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_SEQTRACK = (1 << 3),
    DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_STRMCHANNEL = (1 << 4),

    DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_INSTANCE =
        DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_SEQSOUND |
        DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_STRMSOUND |
        DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_WAVESOUND |
        DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_SEQTRACK |
        DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_STRMCHANNEL
};

u32 gWarningFlag = DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_INSTANCE;

u32 GetWarningBitFlag(DebugWarningFlag warning) {
    u32 bitFlag = 0;

    switch (warning) {
    case DEBUG_WARNING_NOT_ENOUGH_INSTANCE:
        bitFlag = DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_INSTANCE;
        break;

    case DEBUG_WARNING_NOT_ENOUGH_SEQSOUND:
        bitFlag = DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_SEQSOUND;
        break;

    case DEBUG_WARNING_NOT_ENOUGH_STRMSOUND:
        bitFlag = DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_STRMSOUND;
        break;

    case DEBUG_WARNING_NOT_ENOUGH_WAVESOUND:
        bitFlag = DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_WAVESOUND;
        break;

    case DEBUG_WARNING_NOT_ENOUGH_SEQTRACK:
        bitFlag = DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_SEQTRACK;
        break;

    case DEBUG_WARNING_NOT_ENOUGH_STRMCHANNEL:
        bitFlag = DEBUG_WARNING_BIT_FLAG_NOT_ENOUGH_STRMCHANNEL;
        break;
    }

    return bitFlag;
}

}

void Debug_SetWarningFlag(DebugWarningFlag warning, bool enable) {
    u32 bitFlag = GetWarningBitFlag(warning);

    if (enable) {
        gWarningFlag |= bitFlag;
    } else {
        gWarningFlag &= ~bitFlag;
    }
}

}
}
