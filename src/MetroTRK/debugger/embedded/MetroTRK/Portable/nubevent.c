#include <MetroTRK/Portable/nubevent.h>
#include <MetroTRK/Portable/mem_TRK.h>
#include <MetroTRK/Portable/msgbuf.h>

extern void OSReport(const char* message);

TRKEventQueue gTRKEventQueue;

DSError TRKInitializeEventQueue(void) {
    gTRKEventQueue.count = 0;
    gTRKEventQueue.next = 0;
    gTRKEventQueue.eventID = 0x100;
    return DS_NoError;
}

BOOL TRKGetNextEvent(TRKEvent* event) {
    BOOL status = FALSE;
    if (0 < gTRKEventQueue.count) {
        TRK_memcpy(event, &gTRKEventQueue.events[gTRKEventQueue.next], sizeof(TRKEvent));
        gTRKEventQueue.count--;
        if (++gTRKEventQueue.next == 2) {
            gTRKEventQueue.next = 0;
        }
        status = TRUE;
    }
    return status;
}

DSError TRKPostEvent(TRKEvent* event) {
    DSError ret = DS_NoError;
    int nextIndex;
    if (gTRKEventQueue.count == 2) {
        ret = DS_EventQueueFull;
        OSReport("MetroTRK - Event Queue full\n");
    } else {
        nextIndex = (gTRKEventQueue.next + gTRKEventQueue.count) % 2;
        TRK_memcpy(&gTRKEventQueue.events[nextIndex], event, sizeof(TRKEvent));
        gTRKEventQueue.events[nextIndex].eventID = gTRKEventQueue.eventID;

        if (++gTRKEventQueue.eventID < 0x100) {
            gTRKEventQueue.eventID = 0x100;
        }

        gTRKEventQueue.count++;
    }

    return ret;
}

void TRKConstructEvent(TRKEvent* event, NubEventType eventType) {
    event->eventType = eventType;
    event->eventID = 0;
    event->msgBufID = -1;
}

void TRKDestructEvent(TRKEvent* event) {
    TRKReleaseBuffer(event->msgBufID);
}
