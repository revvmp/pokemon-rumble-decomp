#include <revolution/fa/puh_ker.h>
#include <revolution/fa/puh_ker_mem.h>
#include <revolution/fa/puh_msc.h>

void* uhg_msc_memid;
static int st_uhs_msc_msgid;
static int st_uhs_msc_status;

static s32 _uhf_msc_api_send_message(UHFMscPacket* p_packet, u32 command);

s32 usbh_msc_read10(void* p_unit, u32 lba, u16 num_block, void* p_buffer, u8* p_sense) {
    UHFMscPacket packet = {0};

    if (st_uhs_msc_status != 1) {
        return -3;
    }
    packet.p_unit = p_unit;
    packet.lba = lba;
    packet.num_block = num_block;
    packet.p_buffer = p_buffer;
    packet.p_sense = p_sense;
    return _uhf_msc_api_send_message(&packet, UHF_MSC_COMMAND_READ10);
}

s32 usbh_msc_write10(void* p_unit, u32 lba, u16 num_block, void* p_buffer, u8* p_sense) {
    UHFMscPacket packet = {0};

    if (st_uhs_msc_status != 1) {
        return -3;
    }
    packet.p_unit = p_unit;
    packet.lba = lba;
    packet.num_block = num_block;
    packet.p_buffer = p_buffer;
    packet.p_sense = p_sense;
    return _uhf_msc_api_send_message(&packet, UHF_MSC_COMMAND_WRITE10);
}

static s32 _uhf_msc_api_send_message(UHFMscPacket* p_packet, u32 command) {
    s32 result = 0;
    s32 semaphore;
    UHFMscMessage* p_message;

    p_message = (UHFMscMessage*)uhf_ker_get_memory_block(uhg_msc_memid, sizeof(UHFMscMessage), 4);
    if (p_message == NULL) {
        return -12;
    }
    semaphore = uhf_ker_create_sem(0, 0);
    if (semaphore <= 0) {
        uhf_ker_release_memory_block(uhg_msc_memid, p_message);
        return -12;
    }
    p_message->command = command;
    p_message->semaphore = semaphore;
    p_message->p_result = &result;
    p_message->packet = *p_packet;
    if (uhf_ker_send_message(st_uhs_msc_msgid, p_message, 0) != 0) {
        uhf_ker_release_memory_block(uhg_msc_memid, p_message);
        uhf_ker_delete_sem(semaphore);
        return -12;
    }
    if (uhf_ker_get_sem(semaphore) != 0) {
        uhf_ker_delete_sem(semaphore);
        return -12;
    }
    if (uhf_ker_delete_sem(semaphore) != 0) {
        return -12;
    }
    return result;
}

s32 uhf_msc_get_message_id(void) {
    return st_uhs_msc_msgid;
}
