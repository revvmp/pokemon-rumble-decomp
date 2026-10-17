#include "revolution/os.h"
#include "revolution/os/OSThread.h"

void OSInitSemaphore(OSSemaphore* semaphore, s32 count) {
    BOOL enabled = OSDisableInterrupts();
    OSInitThreadQueue(&semaphore->queue);
    semaphore->count = count;
    OSRestoreInterrupts(enabled);
}

s32 OSWaitSemaphore(OSSemaphore* semaphore) {
    BOOL enabled = OSDisableInterrupts();
    s32 count;

    while ((count = semaphore->count) <= 0) {
        OSSleepThread(&semaphore->queue);
    }

    semaphore->count--;
    OSRestoreInterrupts(enabled);
    return count;
}

s32 OSTryWaitSemaphore(OSSemaphore* semaphore) {
    s32 count;
    BOOL enabled = OSDisableInterrupts();
    count = semaphore->count;

    if (count > 0) {
        semaphore->count = count - 1;
    }

    OSRestoreInterrupts(enabled);
    return count;
}

s32 OSSignalSemaphore(OSSemaphore* semaphore) {
    BOOL enabled = OSDisableInterrupts();
    s32 count;
    count = semaphore->count++;
    OSWakeupThread(&semaphore->queue);
    OSRestoreInterrupts(enabled);
    return count;
}
