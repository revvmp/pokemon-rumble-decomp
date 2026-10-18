#include <game/VideoTiming.h>

#include <revolution/vi.h>

u32 GetVideoRetraceCount() { return VIGetRetraceCount(); }

void WaitForVideoRetrace() { VIWaitForRetrace(); }

int GetVideoRetraceTicks() {
    int rate = VIGetTvFormat() == 1 ? 50 : 60;
    return 3000 / rate;
}

int GetNominalVideoRetraceTicks() { return 50; }
