#include <revolution/bte/gki.h>
#include <revolution/bte/l2c_int.h>
#include <revolution/bte/transport.h>
#include <revolution/bte/uusb.h>
#include <revolution/ipc/ipcclt.h>
#include <revolution/os/OSContext.h>
#include <revolution/usb.h>

#define UUSB_VID_NINTENDO 0x057E
#define UUSB_PID_BCM2045 0x0305

#define UUSB_EP_BULK_OUT 0x02
#define UUSB_EP_BULK_IN 0x82
#define UUSB_EP_INTR_IN 0x81

#define UUSB_MAX_PENDING_WRITES 5
#define UUSB_BUF_ALIGN 32
#define UUSB_BUF_RESERVE 0x28

#define UUSB_CMD_POOL_SIZE 660
#define UUSB_CMD_POOL_COUNT 45
#define UUSB_ACL_POOL_SIZE 1800
#define UUSB_ACL_POOL_COUNT 30

#define UUSB_REQTYPE_CLASS_DEVICE 0x20

#define MSG_HC_TO_STACK_HCI_EVT 0x1000
#define MSG_HC_TO_STACK_HCI_ACL 0x1100

enum {
    UUSB_STATE_CLOSED = 0,
    UUSB_STATE_OPEN = 2,
    UUSB_STATE_REGISTERED = 4,
    UUSB_STATE_UNREGISTERED = 5
};

typedef struct {
    IOSFd fd;
    UINT8 reserved[12];
    UINT8 bulk_out_ep;
    UINT8 bulk_in_ep;
    UINT8 intr_in_ep;
    UINT8 intr_out_ep;
    UINT32 vid;
    UINT32 pid;
    UINT8 cmd_buffer_pool;
    UINT8 acl_buffer_pool;
    tUUSB_CBACK* p_cback;
    tUUSB* p_cfg;
    UINT8 trace_state;
    UINT8 state;
    UINT8 reserved2;
    UINT8 reading_intr_data;
    BUFFER_Q bulk_buffer_q;
    UINT8 bulk_pending;
    BUFFER_Q intr_buffer_q;
    UINT8 intr_pending;
} tUUSB_CB;

static tUUSB_CB usb;
UINT8 __uusb_ppc_stack1[0x1000] ATTRIBUTE_ALIGN(32);
UINT8 __uusb_ppc_stack2[0x1000] ATTRIBUTE_ALIGN(32);

volatile UINT32 wait4hci = TRUE;
static IOSFd __ntd_ios_file_descriptor = -1;

static UINT32 __ntd_pid;
static UINT32 __ntd_vid;
static BOOLEAN __ntd_pid_vid_specified;
UINT32 __ntd_ohci_init_flag;
UINT32 __ntd_ohci;
static BOOLEAN uusb_g_trace_state_initialized;
static BOOLEAN uusb_g_usb_devid_found;

static void uusb_ReadIntrDataCB(IOSError result, void* p_data);
static void uusb_ReadBulkDataCB(IOSError result, void* p_data);

static void uusb_set_trace_state(UINT8 state) {
    usb.trace_state = state;
    uusb_g_trace_state_initialized = TRUE;
}

static void uusb_ReleaseCmdBufferPool(void) {
    GKI_delete_pool(usb.cmd_buffer_pool);
    usb.cmd_buffer_pool = GKI_INVALID_POOL;
}

static void uusb_ReleaseAclBufferPool(void) {
    GKI_delete_pool(usb.acl_buffer_pool);
    usb.acl_buffer_pool = GKI_INVALID_POOL;
}

static void uusb_CloseDeviceCB(IOSError result, void* p_data) {
    usb.fd = 0;
    usb.bulk_out_ep = 0;
    usb.bulk_in_ep = 0;
    usb.intr_in_ep = 0;
    usb.intr_out_ep = 0;
    uusb_g_usb_devid_found = FALSE;

    IUSB_CloseLib();
    GKI_disable();
    usb.state = UUSB_STATE_UNREGISTERED;
    usb.trace_state = 0;
    uusb_g_trace_state_initialized = FALSE;
    GKI_enable();

    if (usb.p_cback) {
        usb.p_cback(UUSB_EVT_CLOSE, (INT8)result);
    }
}

static void uusb_issue_intr_read(void) {
    HC_BT_HDR* p_buf;
    UINT8* p_buffer;
    IOSError ret;

    do {
        p_buf = (HC_BT_HDR*)GKI_getpoolbuf(usb.cmd_buffer_pool);
    } while (!p_buf);

    p_buf->event = MSG_HC_TO_STACK_HCI_EVT;
    p_buf->len = 0;
    p_buffer = (UINT8*)(((UINT32)(p_buf + 1) + UUSB_BUF_ALIGN - 1) & ~(UUSB_BUF_ALIGN - 1));
    p_buf->offset = (UINT16)(p_buffer - (UINT8*)(p_buf + 1));

    ret = IUSB_ReadIntrMsgAsync(usb.fd, usb.intr_in_ep, GKI_get_buf_size(p_buf) - UUSB_BUF_RESERVE - p_buf->offset,
                                (char*)p_buffer, uusb_ReadIntrDataCB, p_buf);
    if (ret != 0) {
        GKI_freebuf(p_buf);
    }

    usb.reading_intr_data = TRUE;
}

static void uusb_issue_bulk_read(void) {
    HC_BT_HDR* p_buf;
    UINT8* p_buffer;
    IOSError ret;

    do {
        p_buf = (HC_BT_HDR*)GKI_getpoolbuf(usb.acl_buffer_pool);
    } while (!p_buf);

    p_buf->event = MSG_HC_TO_STACK_HCI_ACL;
    p_buf->len = 0;
    p_buffer = (UINT8*)(((UINT32)(p_buf + 1) + UUSB_BUF_ALIGN - 1) & ~(UUSB_BUF_ALIGN - 1));
    p_buf->offset = (UINT16)(p_buffer - (UINT8*)(p_buf + 1));

    ret = IUSB_ReadBlkMsgAsync(usb.fd, usb.bulk_in_ep, GKI_get_buf_size(p_buf) - UUSB_BUF_RESERVE - p_buf->offset,
                               (char*)p_buffer, uusb_ReadBulkDataCB, p_buf);
    if (ret != 0) {
        GKI_freebuf(p_buf);
    }
}

static void uusb_ReadIntrDataCB(IOSError result, void* p_data) {
    UINT32 bytes_read = 0;
    UINT32 len;
    void* p_msg = NULL;
    HC_BT_HDR* p_buf = (HC_BT_HDR*)p_data;

    len = 0;
    usb.reading_intr_data = FALSE;

    if (usb.state != UUSB_STATE_OPEN) {
        GKI_freebuf(p_buf);
        uusb_ReleaseCmdBufferPool();
        return;
    }

    if (p_buf == NULL) {
        goto end;
    }

    if (result <= 0) {
        GKI_freebuf(p_buf);
        goto end;
    }

    p_msg = GKI_getpoolbuf(GKI_POOL_ID_2);
    if (!p_msg) {
        GKI_freebuf(p_buf);
        goto end;
    }

    p_buf->event = MSG_HC_TO_STACK_HCI_EVT;
    p_buf->len = (UINT16)result;
    len = sizeof(HC_BT_HDR) + p_buf->len + p_buf->offset;
    len = (len >> 2 << 2) + 4;
    memcpy(p_msg, p_buf, len);

    OSSwitchFiberEx((u32)p_msg, 0, 0, 0, (u32)bta_ci_hci_msg_handler, (u32)(__uusb_ppc_stack1 + sizeof(__uusb_ppc_stack1)));
    GKI_freebuf(p_buf);

end:
    uusb_issue_intr_read();
}

static void uusb_ReadBulkDataCB(IOSError result, void* p_data) {
    UINT32 len;
    void* p_msg = NULL;
    HC_BT_HDR* p_buf = (HC_BT_HDR*)p_data;

    len = 0;

    if (usb.state != UUSB_STATE_OPEN) {
        GKI_freebuf(p_buf);
        uusb_ReleaseAclBufferPool();
        return;
    }

    if (result <= 0) {
        GKI_freebuf(p_buf);
        goto end;
    }

    p_buf->len = (UINT16)result;

    p_msg = GKI_getpoolbuf(GKI_POOL_ID_3);
    if (!p_msg) {
        GKI_freebuf(p_buf);
        goto end;
    }

    len = sizeof(HC_BT_HDR) + p_buf->len + p_buf->offset;
    len = (len >> 2 << 2) + 4;
    memcpy(p_msg, p_buf, len);

    p_msg = l2cap_link_chk_pkt_start((BT_HDR*)p_msg);
    if (p_msg != NULL && l2cap_link_chk_pkt_end()) {
        OSSwitchFiberEx((u32)p_msg, 0, 0, 0, (u32)bta_ci_hci_msg_handler, (u32)(__uusb_ppc_stack2 + sizeof(__uusb_ppc_stack2)));
    }

    GKI_freebuf(p_buf);

end:
    uusb_issue_bulk_read();
}

static void uusb_WriteCtrlDataCB(IOSError result, void* p_data) {
    HC_BT_HDR* p_buf;
    IOSError ret;

    if (p_data != NULL) {
        GKI_freebuf(p_data);
        GKI_disable();
        usb.intr_pending--;
        GKI_enable();
    }

    if (usb.intr_pending >= UUSB_MAX_PENDING_WRITES) {
        return;
    }

    if (usb.intr_buffer_q.count == 0) {
        return;
    }

    p_buf = (HC_BT_HDR*)GKI_dequeue(&usb.intr_buffer_q);
    ret = IUSB_WriteCtrlMsgAsync(usb.fd, UUSB_REQTYPE_CLASS_DEVICE, 0, 0, 0, p_buf->len,
                                 (char*)(p_buf + 1) + p_buf->offset, uusb_WriteCtrlDataCB, p_buf);
    if (ret < 0) {
        GKI_enqueue_head(&usb.intr_buffer_q, p_buf);
    } else {
        GKI_disable();
        usb.intr_pending++;
        GKI_enable();
    }
}

static void uusb_WriteBulkDataCB(IOSError result, void* p_data) {
    HC_BT_HDR* p_buf;
    IOSError ret;

    if (p_data != NULL) {
        GKI_freebuf(p_data);
        GKI_disable();
        usb.bulk_pending--;
        GKI_enable();
    }

    if (usb.bulk_pending >= UUSB_MAX_PENDING_WRITES) {
        return;
    }

    if (usb.bulk_buffer_q.count == 0) {
        return;
    }

    p_buf = (HC_BT_HDR*)GKI_dequeue(&usb.bulk_buffer_q);
    ret = IUSB_WriteBlkMsgAsync(usb.fd, usb.bulk_out_ep, p_buf->len, (char*)(p_buf + 1) + p_buf->offset,
                                uusb_WriteBulkDataCB, p_buf);
    if (ret < 0) {
        GKI_enqueue_head(&usb.bulk_buffer_q, p_buf);
    } else {
        GKI_disable();
        usb.bulk_pending++;
        GKI_enable();
    }
}

static IOSError uusb_get_devId(UINT32 vid, UINT32 pid) {
    IOSError ret;

    if (__ntd_ohci_init_flag == TRUE) {
        if (__ntd_ohci == 0) {
            ret = IUSB_OpenDeviceIds("oh0", (u16)vid, (u16)pid, &usb.fd);
        } else if (__ntd_ohci == 1) {
            ret = IUSB_OpenDeviceIds("oh1", (u16)vid, (u16)pid, &usb.fd);
        }
    } else {
        ret = IUSB_OpenDeviceIds("oh1", (u16)vid, (u16)pid, &usb.fd);
    }

    if (ret >= 0) {
        __ntd_ios_file_descriptor = ret;
    }

    return ret;
}

void UUSB_Register(tUUSB* p_uusb) {
    memset(&usb, 0, sizeof(usb));

    GKI_disable();
    usb.state = UUSB_STATE_UNREGISTERED;
    GKI_enable();

    if (!uusb_g_trace_state_initialized) {
        uusb_set_trace_state(0);
    }

    if (IPCCltInit() != 0) {
        return;
    }

    if (IUSB_OpenLib() != 0) {
        return;
    }

    usb.p_cfg = p_uusb;

    if (__ntd_pid_vid_specified == TRUE) {
        usb.vid = __ntd_vid;
        usb.pid = __ntd_pid;
    } else {
        usb.vid = UUSB_VID_NINTENDO;
        usb.pid = UUSB_PID_BCM2045;
    }

    usb.bulk_out_ep = 0;
    usb.bulk_in_ep = 0;
    usb.intr_in_ep = 0;
    usb.intr_out_ep = 0;

    if (uusb_get_devId(usb.vid, usb.pid) < 0) {
        return;
    }

    usb.bulk_out_ep = UUSB_EP_BULK_OUT;
    usb.bulk_in_ep = UUSB_EP_BULK_IN;
    usb.intr_in_ep = UUSB_EP_INTR_IN;
    usb.intr_out_ep = 0;

    GKI_init_q(&usb.bulk_buffer_q);
    usb.bulk_pending = 0;
    GKI_init_q(&usb.intr_buffer_q);
    usb.intr_pending = 0;

    usb.cmd_buffer_pool = GKI_create_pool(UUSB_CMD_POOL_SIZE, UUSB_CMD_POOL_COUNT, GKI_RESTRICTED_POOL, NULL);
    usb.acl_buffer_pool = GKI_create_pool(UUSB_ACL_POOL_SIZE, UUSB_ACL_POOL_COUNT, GKI_RESTRICTED_POOL, NULL);

    if (usb.cmd_buffer_pool == GKI_INVALID_POOL || usb.acl_buffer_pool == GKI_INVALID_POOL) {
        return;
    }

    GKI_disable();
    usb.state = UUSB_STATE_REGISTERED;
    GKI_enable();

    wait4hci = TRUE;
}

void UUSB_Open(tUUSB* p_uusb, tUUSB_CBACK* p_cback) {
    if (usb.state != UUSB_STATE_REGISTERED) {
        return;
    }

    if (usb.cmd_buffer_pool == GKI_INVALID_POOL || usb.acl_buffer_pool == GKI_INVALID_POOL) {
        return;
    }

    GKI_disable();
    usb.p_cback = p_cback;
    usb.state = UUSB_STATE_OPEN;
    GKI_enable();

    uusb_issue_intr_read();
    uusb_issue_bulk_read();

    wait4hci = FALSE;
}

UINT16 UUSB_Read(UINT8 port, void* p_data, UINT16 len) {
    return 0;
}

UINT16 UUSB_Write(UINT8 port, void* p_data, UINT16 len, void* p_msg) {
    UINT16 bytes_sent = 0;
    IOSError ret = 0;
    UINT8* p_buffer = NULL;
    HC_BT_HDR* p_buf = NULL;

    if (usb.state != UUSB_STATE_OPEN) {
        return 0;
    }

    switch (port) {
    case UUSB_PORT_CMD:
        p_buf = (HC_BT_HDR*)GKI_getpoolbuf(usb.cmd_buffer_pool);
        if (p_buf == NULL) {
            return 0;
        }

        p_buf->len = len;
        p_buffer = (UINT8*)(((UINT32)(p_buf + 1) + UUSB_BUF_ALIGN - 1) & ~(UUSB_BUF_ALIGN - 1));
        p_buf->offset = (UINT16)(p_buffer - (UINT8*)(p_buf + 1));
        memcpy(p_buffer, p_data, len);

        if (usb.intr_pending < UUSB_MAX_PENDING_WRITES && usb.intr_buffer_q.count == 0) {
            ret = IUSB_WriteCtrlMsgAsync(usb.fd, UUSB_REQTYPE_CLASS_DEVICE, 0, 0, 0, len, (char*)p_buffer,
                                         uusb_WriteCtrlDataCB, p_buf);
        } else {
            GKI_enqueue(&usb.intr_buffer_q, p_buf);
            return 0;
        }

        if (ret != 0) {
            GKI_freebuf(p_buf);
        } else {
            GKI_disable();
            usb.intr_pending++;
            GKI_enable();
        }
        break;

    case UUSB_PORT_ACL:
        p_buf = (HC_BT_HDR*)GKI_getpoolbuf(usb.acl_buffer_pool);
        if (p_buf == NULL) {
            return 0;
        }

        p_buf->len = len;
        p_buffer = (UINT8*)(((UINT32)(p_buf + 1) + UUSB_BUF_ALIGN - 1) & ~(UUSB_BUF_ALIGN - 1));
        p_buf->offset = (UINT16)(p_buffer - (UINT8*)(p_buf + 1));
        memcpy(p_buffer, p_data, len);

        if (usb.bulk_pending < UUSB_MAX_PENDING_WRITES && usb.bulk_buffer_q.count == 0) {
            ret = IUSB_WriteBlkMsgAsync(usb.fd, usb.bulk_out_ep, len, (char*)p_buffer, uusb_WriteBulkDataCB, p_buf);
        } else {
            GKI_enqueue(&usb.bulk_buffer_q, p_buf);
            return 0;
        }

        if (ret != 0) {
            GKI_freebuf(p_buf);
        } else {
            GKI_disable();
            usb.bulk_pending++;
            GKI_enable();
        }
        break;

    case UUSB_PORT_SCO:
        break;
    }

    return (UINT16)ret;
}

void UUSB_Close(void) {
    GKI_disable();
    usb.state = UUSB_STATE_CLOSED;
    GKI_enable();

    if (usb.bulk_buffer_q.count) {
        while (usb.bulk_buffer_q.count) {
            GKI_freebuf(GKI_dequeue(&usb.bulk_buffer_q));
        }
    }

    if (usb.intr_buffer_q.count) {
        while (usb.intr_buffer_q.count) {
            GKI_freebuf(GKI_dequeue(&usb.intr_buffer_q));
        }
    }

    IUSB_CloseDeviceAsync(usb.fd, uusb_CloseDeviceCB, NULL);
}

void UUSB_Unregister(void) {
    IUSB_CloseLib();

    GKI_disable();
    usb.state = UUSB_STATE_UNREGISTERED;
    usb.trace_state = 0;
    uusb_g_trace_state_initialized = FALSE;
    GKI_enable();
}
