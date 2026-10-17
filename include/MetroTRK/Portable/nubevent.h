#ifndef METROTRK_PORTABLE_NUBEVENT_H
#define METROTRK_PORTABLE_NUBEVENT_H

#include <MetroTRK/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 NubEventID;

typedef struct TRKEvent {
    NubEventType eventType;
    NubEventID eventID;
    MessageBufferID msgBufID;
} TRKEvent;

BOOL TRKGetNextEvent(TRKEvent* event);
void TRKDestructEvent(TRKEvent* event);

#ifdef __cplusplus
}
#endif

#endif
