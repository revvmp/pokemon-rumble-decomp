#ifndef FA_ISD_H
#define FA_ISD_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISD_EVENT_INSERT 1
#define ISD_EVENT_REMOVE 2

#define ISD_STATUS_INSERTED 1

#define ISD_CMD_SEND_RELATIVE_ADDR 3
#define ISD_CMD_SEND_CID 10
#define ISD_CMD_SD_SEND_OP_COND 41

typedef struct ISD_HANDLE {
    s32 fd;
    u32 reserved1[3];
    u32 rca;
    u32 reserved2;
    u32 capacity[3];
    u32 opened;
} ISD_HANDLE;

typedef s32 (*ISD_INTR_HANDLER)(s32 event, void* p_context);

s32 ISD_InitCard(void);
s32 ISD_MountCard(u32 slot, ISD_HANDLE** pp_handle);
s32 ISD_UnmountCard(ISD_HANDLE* p_handle);
s32 ISD_GetDeviceStatus(ISD_HANDLE* p_handle, u32* p_status);
s32 ISD_ResetDevice(ISD_HANDLE* p_handle);
s32 ISD_ReadCardRegister(ISD_HANDLE* p_handle, u32 command, u32* p_response, u32 size);
s32 ISD_GetCardSize(ISD_HANDLE* p_handle, u32* p_total_blocks, u32* p_block_count, u32* p_block_size);
s32 ISD_RegisterDeviceIntrHandler(ISD_HANDLE* p_handle, ISD_INTR_HANDLER handler, u32* p_event);
s32 ISD_UnregisterDeviceIntrHandler(ISD_HANDLE* p_handle);

#ifdef __cplusplus
}
#endif

#endif
