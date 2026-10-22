#ifndef FA_PUH_MSC_H
#define FA_PUH_MSC_H

#include <revolution/types.h>

typedef struct UHFMscPacket {
    void* p_unit;
    u8* p_sense;
    u32 reserved0;
    void* p_buffer;
    u32 reserved1;
    u32 lba;
    u16 reserved2;
    u16 num_block;
    u32 reserved3[2];
    void* p_disk;
} UHFMscPacket;

typedef struct UHFMscMessage {
    u32 command;
    s32 semaphore;
    s32* p_result;
    UHFMscPacket packet;
    u32 reserved[2];
} UHFMscMessage;

#define UHF_MSC_COMMAND_INIT 0x01010005
#define UHF_MSC_COMMAND_FINALIZE 0x01010006
#define UHF_MSC_COMMAND_READ10 0x01010A00
#define UHF_MSC_COMMAND_WRITE10 0x01010B00

s32 usbh_msc_read10(void* p_unit, u32 lba, u16 num_block, void* p_buffer, u8* p_sense);
s32 usbh_msc_write10(void* p_unit, u32 lba, u16 num_block, void* p_buffer, u8* p_sense);
s32 uhf_msc_get_message_id(void);

#endif
