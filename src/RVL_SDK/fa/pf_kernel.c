#include <revolution/fa/pf_kernel.h>
#include <revolution/os.h>

OSThread* pfk_create_task(PFK_TASK_PARAMS* p_params) {
    if (p_params == NULL) {
        OSReport("prfile2:ERR pfk_create_task() invalid param\n");
        return NULL;
    }

    if (!OSCreateThread(p_params->thread, p_params->entry, p_params->argument,
                        p_params->stack + p_params->stack_size, p_params->stack_size,
                        p_params->priority, 0)) {
        OSReport("prfile2:ERR pfk_create_task() failed to create thread\n");
        return NULL;
    }

    return p_params->thread;
}

s32 pfk_start_task(OSThread* p_thread) {
    if (p_thread == NULL) {
        OSReport("prfile2:ERR pfk_start_task() invalid tsk_id\n");
        return -2;
    }

    if (OSIsThreadSuspended(p_thread) == 1) {
        OSResumeThread(p_thread);
    }

    return 0;
}

s32 pfk_get_task_id(OSThread** pp_thread) {
    OSThread* p_thread;

    if (pp_thread == NULL) {
        OSReport("prfile2:ERR pfk_get_task_id() invalid param\n");
        return -2;
    }

    p_thread = OSGetCurrentThread();
    if (p_thread == NULL) {
        OSReport("INFO failed to OSGetCurrentThread\n");
        *pp_thread = NULL;
        return -3;
    }

    *pp_thread = p_thread;
    return 0;
}

OSMessageQueue* pfk_create_mailbox(PFK_MAILBOX_PARAMS* p_params) {
    if (p_params == NULL || p_params->queue == NULL || p_params->messages == NULL) {
        OSReport("prfile2:ERR pfk_create_mailbox() Invalid Param\n");
        return NULL;
    }

    OSInitMessageQueue(p_params->queue, p_params->messages, p_params->capacity);
    return p_params->queue;
}

s32 pfk_receive_message(OSMessageQueue* p_queue, OSMessage* p_message) {
    if (p_queue == NULL) {
        OSReport("prfile2:ERR pfk_receive_message() Invalid Param\n");
        return -2;
    }

    if (!OSReceiveMessage(p_queue, p_message, OS_MESSAGE_BLOCK)) {
        OSReport("prfile2:ERR pfk_receive_message() Faile to OSReceiveMessage()\n");
        return -3;
    }

    return 0;
}

s32 pfk_send_message(OSMessageQueue* p_queue, OSMessage message) {
    if (p_queue == NULL || message == NULL) {
        OSReport("prfile2:ERR pfk_send_message() Invalid Param\n");
        return -2;
    }

    if (!OSSendMessage(p_queue, message, OS_MESSAGE_NOBLOCK)) {
        OSReport("prfile2:ERR pfk_send_message() Failed to pfk_send_message()\n");
        return -3;
    }

    return 0;
}

OSSemaphore* pfk_create_semaphore(PFK_SEMAPHORE_PARAMS* p_params) {
    s32 remaining;

    if (p_params == NULL || p_params->semaphore == NULL || p_params->initial_count > p_params->maximum_count) {
        OSReport("prfile2:ERR pfk_create_semaphore() Invalid Param\n");
        return NULL;
    }

    OSInitSemaphore(p_params->semaphore, p_params->maximum_count);
    remaining = p_params->maximum_count - p_params->initial_count;
    while (remaining != 0) {
        OSWaitSemaphore(p_params->semaphore);
        remaining--;
    }

    return p_params->semaphore;
}

s32 pfk_delete_semaphore(OSSemaphore* p_semaphore) {
    if (p_semaphore == NULL) {
        OSReport("prfile2:ERR pfk_delete_semaphore() Invalid Param\n");
        return -2;
    }

    return 0;
}

s32 pfk_get_semaphore(OSSemaphore* p_semaphore) {
    if (p_semaphore == NULL) {
        OSReport("prfile2:ERR pfk_get_semaphore() Invalid Param\n");
        return -2;
    }

    OSWaitSemaphore(p_semaphore);
    return 0;
}

s32 pfk_release_semaphore(OSSemaphore* p_semaphore) {
    if (p_semaphore == NULL) {
        OSReport("prfile2:ERR pfk_release_semaphore() Invalid Param\n");
        return -2;
    }

    OSSignalSemaphore(p_semaphore);
    return 0;
}
