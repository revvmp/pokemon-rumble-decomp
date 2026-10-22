#ifndef FA_PUH_KER_H
#define FA_PUH_KER_H

#include <revolution/os.h>
#include <revolution/os/OSMessage.h>

#define UHF_KER_MESSAGE_QUEUE_MAX 2
#define UHF_KER_SEMAPHORE_MAX 12

s32 uhf_ker_send_message(int queue_id, OSMessage message, s32 options);
s32 uhf_ker_create_sem(u32 initial_count, int attribute);
s32 uhf_ker_delete_sem(int semaphore_id);
s32 uhf_ker_get_sem(int semaphore_id);

#endif
