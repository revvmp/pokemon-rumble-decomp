#include <game/GameLoop.h>

#include <game/FrameStep.h>
#include <game/VideoTiming.h>

int gGameFrame;
int gFrameStep;
bool gFrameTimingReset;

namespace {

const int RENDER_HISTORY_SIZE = 60;

int sRenderCount;
int sRenderHistoryIndex;
bool sRenderHistory[RENDER_HISTORY_SIZE];

}

void RunGameLoop(IGameLoopCallback* pCallback) {
    u32 retraceCount = GetVideoRetraceCount();
    int retraceTicks = GetVideoRetraceTicks();
    int nominalTicks = GetNominalVideoRetraceTicks();
    int elapsedTicks = 0;
    int budgetTicks = 0;
    bool resync = false;
    gFrameTimingReset = true;

    while (pCallback->IsRunning()) {
        int updates = 0;
        int rendered = 0;

        if (resync) {
            if (elapsedTicks - budgetTicks > 0) {
                updates = (elapsedTicks - budgetTicks) / nominalTicks;
            }
            elapsedTicks = 0;
            budgetTicks = 0;
            retraceCount = GetVideoRetraceCount();
            resync = false;
            pCallback->ResetTiming();
        }

        for (;;) {
            budgetTicks += nominalTicks;
            pCallback->Update();
            updates++;
            if (pCallback->ShouldRender()) {
                pCallback->Prepare();
                gGameFrame++;
                rendered++;
            }

            u32 now = GetVideoRetraceCount();
            elapsedTicks += retraceTicks * (now - retraceCount);
            retraceCount = now;

            if (elapsedTicks > budgetTicks) {
                bool wasReset = gFrameTimingReset;
                gFrameTimingReset = false;
                if (!wasReset) {
                    continue;
                }
                resync = true;
            }
            break;
        }

        for (int i = 0; i < updates; i++) {
            if (sRenderHistory[sRenderHistoryIndex]) {
                sRenderCount--;
            }
            sRenderHistory[sRenderHistoryIndex] = i == updates - 1;
            if (sRenderHistory[sRenderHistoryIndex]) {
                sRenderCount++;
            }
            sRenderHistoryIndex = (sRenderHistoryIndex + 1) % RENDER_HISTORY_SIZE;
        }

        gFrameStep = rendered;
        pCallback->Calc();
        pCallback->Draw();
        pCallback->Render();

        if (!resync) {
            for (;;) {
                u32 now = GetVideoRetraceCount();
                int delta = now - retraceCount;
                elapsedTicks += delta * retraceTicks;
                retraceCount = now;
                if (budgetTicks <= elapsedTicks + retraceTicks) {
                    break;
                }
                WaitForVideoRetrace();
            }
        }

        pCallback->Present();
    }
}
