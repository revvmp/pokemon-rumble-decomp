#include <revolution/fa/puh_ker.h>

typedef struct UHFKerMessageSlot {
    OSMessageQueue* p_queue;
    u32 reserved;
} UHFKerMessageSlot;

static UHFKerMessageSlot g_message_slot[UHF_KER_MESSAGE_QUEUE_MAX + 1];

s32 uhf_ker_send_message(int queue_id, OSMessage message, s32 options) {
    if (queue_id == 0 || queue_id > UHF_KER_MESSAGE_QUEUE_MAX) {
        return -5;
    }
    if (message == NULL) {
        return -5;
    }
    if (g_message_slot[queue_id - 1].p_queue == NULL) {
        return -12;
    }
    if (OSSendMessage(g_message_slot[queue_id - 1].p_queue, message, 0)) {
        return 0;
    }
    return -20;
}
