#ifndef __METROTRK_TRK_H__
#define __METROTRK_TRK_H__

#include <revolution/types.h>

typedef enum {
    DS_NoError = 0x0,
    DS_StepError = 0x1,
    DS_ParameterError = 0x2,

    DS_EventQueueFull = 0x100,

    DS_NoMessageBufferAvailable = 0x300,
    DS_MessageBufferOverflow = 0x301,
    DS_MessageBufferReadError = 0x302,

    DS_DispatchError = 0x500,

    DS_InvalidMemory = 0x700,
    DS_InvalidRegister = 0x701,
    DS_CWDSException = 0x702,
    DS_UnsupportedError = 0x703,
    DS_InvalidProcessID = 0x704,
    DS_InvalidThreadID = 0x705,
    DS_OSError = 0x706,

    DS_Error800 = 0x800,
} DSError;

typedef enum {
    NUBEVENT_Null = 0,
    NUBEVENT_Shutdown = 1,
    NUBEVENT_Request = 2,
    NUBEVENT_Breakpoint = 3,
    NUBEVENT_Exception = 4,
    NUBEVENT_Support = 5,
} NubEventType;

typedef enum {
    DSMSG_Ping = 0x0,
    DSMSG_Connect = 0x1,
    DSMSG_Disconnect = 0x2,
    DSMSG_Reset = 0x3,
    DSMSG_Versions = 0x4,
    DSMSG_SupportMask = 0x5,
    DSMSG_Override = 0x7,

    DSMSG_ReadMemory = 0x10,
    DSMSG_WriteMemory = 0x11,
    DSMSG_ReadRegisters = 0x12,
    DSMSG_WriteRegisters = 0x13,
    DSMSG_SetOption = 0x17,
    DSMSG_Continue = 0x18,
    DSMSG_Step = 0x19,
    DSMSG_Stop = 0x1A,

    DSMSG_ReplyACK = 0x80,

    DSMSG_NotifyStopped = 0x90,
    DSMSG_NotifyException = 0x91,

    DSMSG_WriteFile = 0xD0,
    DSMSG_ReadFile = 0xD1,
    DSMSG_OpenFile = 0xD2,
    DSMSG_CloseFile = 0xD3,
    DSMSG_PositionFile = 0xD4,

    DSMSG_ReplyNAK = 0xFF,
} MessageCommandID;

typedef int MessageBufferID;
typedef struct TRKBuffer TRKBuffer;

#endif
