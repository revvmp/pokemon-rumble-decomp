#ifndef RVL_SDK_BTE_UUSB_H
#define RVL_SDK_BTE_UUSB_H

#include <revolution/bte/data_types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    UUSB_EVT_OPEN = 0,
    UUSB_EVT_CLOSE = 4
};

enum {
    UUSB_PORT_CMD = 0,
    UUSB_PORT_EVT = 1,
    UUSB_PORT_ACL = 2,
    UUSB_PORT_SCO = 3
};

typedef void tUUSB_CBACK(UINT8 event, INT8 status);

typedef struct {
    UINT8 reserved[4];
    UINT16 vid;
    UINT16 pid;
    UINT16 task_id;
    UINT8 unused;
    UINT8 flags;
} tUUSB;

extern volatile UINT32 wait4hci;

void UUSB_Register(tUUSB* p_uusb);
void UUSB_Open(tUUSB* p_uusb, tUUSB_CBACK* p_cback);
UINT16 UUSB_Read(UINT8 port, void* p_data, UINT16 len);
UINT16 UUSB_Write(UINT8 port, void* p_data, UINT16 len, void* p_msg);
void UUSB_Close(void);
void UUSB_Unregister(void);

#ifdef __cplusplus
}
#endif

#endif
