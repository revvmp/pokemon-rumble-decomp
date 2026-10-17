#ifndef OS_DOLPHIN_GDEV_STUBS_H
#define OS_DOLPHIN_GDEV_STUBS_H

#include <revolution/os/OSInterrupt.h>

#ifdef __cplusplus
extern "C" {
#endif

int gdev_cc_initialize(void* inputPendingPtrRef, __OSInterruptHandler monitorCallback);
int gdev_cc_shutdown(void);
int gdev_cc_open(void);
int gdev_cc_close(void);
int gdev_cc_read(u8* data, int size);
int gdev_cc_write(const u8* bytes, int length);
int gdev_cc_pre_continue(void);
int gdev_cc_post_stop(void);
int gdev_cc_peek(void);
int gdev_cc_initinterrupts(void);

#ifdef __cplusplus
}
#endif

#endif
