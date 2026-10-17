#ifndef NW4R_SND_DEBUG_H
#define NW4R_SND_DEBUG_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace snd {

enum DebugWarningFlag {
    DEBUG_WARNING_NOT_ENOUGH_INSTANCE,
    DEBUG_WARNING_NOT_ENOUGH_SEQSOUND,
    DEBUG_WARNING_NOT_ENOUGH_STRMSOUND,
    DEBUG_WARNING_NOT_ENOUGH_WAVESOUND,
    DEBUG_WARNING_NOT_ENOUGH_SEQTRACK,
    DEBUG_WARNING_NOT_ENOUGH_STRMCHANNEL
};

void Debug_SetWarningFlag(DebugWarningFlag warning, bool enable);

}
}

#endif
