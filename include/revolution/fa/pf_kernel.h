#ifndef FA_PF_KERNEL_H
#define FA_PF_KERNEL_H

#include <revolution/os/OSThread.h>
#include <revolution/os/OSMessage.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PFK_TASK_PARAMS {
    void* (*entry)(void*);
    void* argument;
    u32 stack_size;
    u8* stack;
    OSPriority priority;
    OSThread* thread;
} PFK_TASK_PARAMS;

typedef struct PFK_MAILBOX_PARAMS {
    OSMessageQueue* queue;
    OSMessage* messages;
    s32 capacity;
} PFK_MAILBOX_PARAMS;

typedef struct PFK_SEMAPHORE_PARAMS {
    u32 initial_count;
    u32 maximum_count;
    OSSemaphore* semaphore;
} PFK_SEMAPHORE_PARAMS;

OSThread* pfk_create_task(PFK_TASK_PARAMS* p_params);
s32 pfk_start_task(OSThread* p_thread);
s32 pfk_get_task_id(OSThread** pp_thread);
OSMessageQueue* pfk_create_mailbox(PFK_MAILBOX_PARAMS* p_params);
s32 pfk_receive_message(OSMessageQueue* p_queue, OSMessage* p_message);
s32 pfk_send_message(OSMessageQueue* p_queue, OSMessage message);
OSSemaphore* pfk_create_semaphore(PFK_SEMAPHORE_PARAMS* p_params);
s32 pfk_delete_semaphore(OSSemaphore* p_semaphore);
s32 pfk_get_semaphore(OSSemaphore* p_semaphore);
s32 pfk_release_semaphore(OSSemaphore* p_semaphore);

#ifdef __cplusplus
}
#endif

#endif
