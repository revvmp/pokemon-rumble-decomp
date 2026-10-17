#include <revolution/bte/bt_trace.h>
#include <revolution/bte/btu.h>
#include <revolution/bte/gki.h>
#include <revolution/bte/l2c_int.h>
#include <revolution/bte/transport.h>
#include <revolution/bte/uusb.h>

#define HCI_CMD_PREAMBLE_SIZE 3
#define HCI_ACL_PREAMBLE_SIZE 4
#define HCI_SCO_PREAMBLE_SIZE 3
#define HCI_EVT_PREAMBLE_SIZE 2

#define H4_TYPE_COMMAND 1
#define H4_TYPE_ACL_DATA 2
#define H4_TYPE_SCO_DATA 3
#define H4_TYPE_EVENT 4

#define HCISU_NUM_RX_PORTS 3

#define MSG_STACK_TO_HC_HCI_CMD 0x2000
#define MSG_STACK_TO_HC_HCI_ACL 0x2100
#define MSG_STACK_TO_HC_HCI_SCO 0x2200
#define MSG_HC_TO_STACK_HCI_ERR 0x1300
#define MSG_HC_TO_STACK_L2C_SEG_XMIT 0x1900

enum {
    HCISU_H2_RX_IDLE,
    HCISU_H2_RX_HEADER,
    HCISU_H2_RX_SKIP_HEADER,
    HCISU_H2_RX_DATA,
    HCISU_H2_RX_SKIP_DATA
};

enum {
    HCISU_H2_CLOSED = 0,
    HCISU_H2_OPEN = 2
};

typedef struct {
    HC_BT_HDR* p_rcv_msg[HCISU_NUM_RX_PORTS];
    UINT16 rcv_len[HCISU_NUM_RX_PORTS];
    UINT16 hdr_offset;
    UINT8 rcv_msg_type[HCISU_NUM_RX_PORTS];
    UINT8 rcv_len_low[HCISU_NUM_RX_PORTS];
    UINT8 rcv_state[HCISU_NUM_RX_PORTS];
    UINT8 reserved;
    UINT8 port_state;
    UINT8 task_id;
    UINT8 mbox;
} tHCISU_H2_CB;

void hcisu_h2_init(UINT8 task_id, UINT8 mbox, UINT16 hdr_offset);
BOOLEAN hcisu_h2_open(tHCI_CFG* p_cfg);
void hcisu_h2_close(void);
BOOLEAN hcisu_h2_send(HC_BT_HDR* p_msg);
UINT32 hcisu_h2_handle_event(UINT16 len);

const tHCI_IF hcisu_h2 = {
    hcisu_h2_init, hcisu_h2_open, hcisu_h2_close, hcisu_h2_send, hcisu_h2_handle_event,
};

static tHCISU_H2_CB hcisu_h2_cb;

static const UINT8 hcisu_preamble_table[] = {
    HCI_CMD_PREAMBLE_SIZE,
    HCI_ACL_PREAMBLE_SIZE,
    HCI_SCO_PREAMBLE_SIZE,
    HCI_EVT_PREAMBLE_SIZE,
};

static const UINT16 hcisu_msg_evt_table[] = {
    MSG_HC_TO_STACK_HCI_ERR,
    BT_EVT_TO_BTU_HCI_ACL,
    BT_EVT_TO_BTU_HCI_SCO,
    BT_EVT_TO_BTU_HCI_EVT,
};

void hcisu_h2_usb_cback(UINT8 event, INT8 status) {
    switch (event) {
    case UUSB_EVT_OPEN:
        break;
    case UUSB_EVT_CLOSE:
        bta_usb_close_evt(status);
        break;
    }
}

UINT16 hcisu_h2_receive_msg(UINT16 port, tHCISU_H2_CB* p_cb) {
    UINT16 bytes_read = 0;
    UINT8 byte;
    UINT16 msg_len;
    UINT16 len;
    BOOLEAN msg_received;
    UINT16 usb_port;

    if (p_cb->port_state != HCISU_H2_OPEN) {
        return 0;
    }

    usb_port = (UINT16)(port + 1);

    while (TRUE) {
        if (UUSB_Read((UINT8)usb_port, &byte, 1) == 0) {
            break;
        }

        bytes_read++;
        msg_received = FALSE;

        switch (p_cb->rcv_state[port]) {
        case HCISU_H2_RX_IDLE:
            if (usb_port == UUSB_PORT_EVT) {
                p_cb->p_rcv_msg[port] = (HC_BT_HDR*)GKI_getpoolbuf(GKI_POOL_ID_2);
                p_cb->rcv_msg_type[port] = H4_TYPE_EVENT;
            } else if (usb_port == UUSB_PORT_ACL) {
                p_cb->p_rcv_msg[port] = (HC_BT_HDR*)GKI_getpoolbuf(GKI_POOL_ID_3);
                p_cb->rcv_msg_type[port] = H4_TYPE_ACL_DATA;
            } else if (usb_port == UUSB_PORT_SCO) {
                p_cb->p_rcv_msg[port] = (HC_BT_HDR*)GKI_getpoolbuf(GKI_POOL_ID_1);
                p_cb->rcv_msg_type[port] = H4_TYPE_SCO_DATA;
            } else {
                break;
            }

            if (p_cb->p_rcv_msg[port]) {
                p_cb->p_rcv_msg[port]->len = 0;
                p_cb->p_rcv_msg[port]->event = hcisu_msg_evt_table[p_cb->rcv_msg_type[port] - 1];
                p_cb->p_rcv_msg[port]->offset = 0;
                p_cb->rcv_state[port] = HCISU_H2_RX_HEADER;
            } else {
                HCI_TRACE_ERROR0("HCIS: Unable to allocate buffer for incoming HCI message.");
                p_cb->rcv_state[port] = HCISU_H2_RX_SKIP_HEADER;
            }

            p_cb->rcv_len[port] = hcisu_preamble_table[p_cb->rcv_msg_type[port] - 1];

        case HCISU_H2_RX_HEADER:
            *((UINT8*)(p_cb->p_rcv_msg[port] + 1) + p_cb->p_rcv_msg[port]->len++) = byte;

            if (--p_cb->rcv_len[port] == 0) {
                msg_len = byte;

                if (p_cb->rcv_msg_type[port] == H4_TYPE_ACL_DATA) {
                    msg_len = (UINT16)((msg_len << 8) + p_cb->rcv_len_low[port]);

                    if ((p_cb->p_rcv_msg[port] = (HC_BT_HDR*)l2cap_link_chk_pkt_start((BT_HDR*)p_cb->p_rcv_msg[port])) == NULL)
                    {
                        p_cb->rcv_len[port] = msg_len;

                        if (msg_len == 0) {
                            p_cb->rcv_state[port] = HCISU_H2_RX_IDLE;
                        } else {
                            p_cb->rcv_state[port] = HCISU_H2_RX_SKIP_DATA;
                        }
                        break;
                    }
                }

                p_cb->rcv_len[port] = msg_len;

                if (sizeof(HC_BT_HDR) + hcisu_preamble_table[p_cb->rcv_msg_type[port] - 1] + msg_len >
                    GKI_get_buf_size(p_cb->p_rcv_msg[port]))
                {
                    GKI_freebuf(p_cb->p_rcv_msg[port]);
                    p_cb->p_rcv_msg[port] = NULL;
                    p_cb->rcv_state[port] = HCISU_H2_RX_SKIP_DATA;
                    HCI_TRACE_ERROR0("HCIS: Invalid length for incoming HCI message.");
                    break;
                }

                if (msg_len != 0) {
                    p_cb->rcv_state[port] = HCISU_H2_RX_DATA;
                } else {
                    msg_received = TRUE;
                    p_cb->rcv_state[port] = HCISU_H2_RX_IDLE;
                }
            } else {
                p_cb->rcv_len_low[port] = byte;
            }
            break;

        case HCISU_H2_RX_SKIP_HEADER:
            if (--p_cb->rcv_len[port] == 0) {
                msg_len = byte;

                if (p_cb->rcv_msg_type[port] == H4_TYPE_ACL_DATA) {
                    msg_len = (UINT16)((msg_len << 8) + p_cb->rcv_len_low[port]);
                }

                p_cb->rcv_len[port] = msg_len;
                p_cb->rcv_state[port] = HCISU_H2_RX_SKIP_DATA;
            } else {
                p_cb->rcv_len_low[port] = byte;
            }
            break;

        case HCISU_H2_RX_DATA:
            *((UINT8*)(p_cb->p_rcv_msg[port] + 1) + p_cb->p_rcv_msg[port]->len++) = byte;
            p_cb->rcv_len[port]--;

            len = UUSB_Read((UINT8)(port + 1), (UINT8*)(p_cb->p_rcv_msg[port] + 1) + p_cb->p_rcv_msg[port]->len,
                            p_cb->rcv_len[port]);
            p_cb->p_rcv_msg[port]->len += len;
            p_cb->rcv_len[port] -= len;
            bytes_read += len;

            if (p_cb->rcv_len[port] == 0) {
                if (p_cb->rcv_msg_type[port] == H4_TYPE_ACL_DATA && !l2cap_link_chk_pkt_end()) {
                    p_cb->rcv_state[port] = HCISU_H2_RX_IDLE;
                } else {
                    msg_received = TRUE;
                    p_cb->rcv_state[port] = HCISU_H2_RX_IDLE;
                }
            }
            break;

        case HCISU_H2_RX_SKIP_DATA:
            if (--p_cb->rcv_len[port] == 0) {
                p_cb->rcv_state[port] = HCISU_H2_RX_IDLE;
            }
            break;
        }

        if (msg_received) {
            GKI_send_msg(hcisu_h2_cb.task_id, 0, p_cb->p_rcv_msg[port]);
            p_cb->p_rcv_msg[port] = NULL;
        }
    }

    return bytes_read;
}

void hcisu_h2_send_msg_now(tHCISU_H2_CB* p_cb, HC_BT_HDR* p_msg) {
    UINT16 handle;
    UINT8* p = (UINT8*)(p_msg + 1) + p_msg->offset;
    UINT8 port;

    if (p_msg->event == MSG_STACK_TO_HC_HCI_ACL) {
        port = UUSB_PORT_ACL;
    } else if (p_msg->event == MSG_STACK_TO_HC_HCI_SCO) {
        port = UUSB_PORT_SCO;
    } else if (p_msg->event == MSG_STACK_TO_HC_HCI_CMD) {
        port = UUSB_PORT_CMD;
    }

    if (p_msg->event == MSG_STACK_TO_HC_HCI_ACL && p_msg->len > btu_cb.hcit_acl_pkt_size) {
        handle = (UINT16)(p[0] + (UINT16)(p[1] << 8));
        p += 2;
        handle = (UINT16)((handle & 0xCFFF) | 0x1000);

        while (p_msg->len > btu_cb.hcit_acl_pkt_size) {
            UUSB_Write(port, p, btu_cb.hcit_acl_pkt_size, p_msg);

            p_msg->offset += btu_cb.hcit_acl_data_size;
            p_msg->len -= btu_cb.hcit_acl_data_size;
            p = (UINT8*)(p_msg + 1) + p_msg->offset;

            UINT16_TO_STREAM(p, handle);

            if (p_msg->len > btu_cb.hcit_acl_pkt_size) {
                UINT16_TO_STREAM(p, btu_cb.hcit_acl_data_size);
            } else {
                UINT16_TO_STREAM(p, p_msg->len - HCI_ACL_PREAMBLE_SIZE);
            }

            if (p_msg->layer_specific && --p_msg->layer_specific == 0) {
                p_msg->event = MSG_HC_TO_STACK_L2C_SEG_XMIT;
                GKI_send_msg(hcisu_h2_cb.task_id, 0, p_msg);
                return;
            }
        }
    }

    UUSB_Write(port, p, p_msg->len, p_msg);
    GKI_freebuf(p_msg);
}

void hcisu_h2_send_msg(tHCISU_H2_CB* p_cb, HC_BT_HDR* p_msg) {
    hcisu_h2_send_msg_now(p_cb, p_msg);
}

void hcisu_h2_init(UINT8 task_id, UINT8 mbox, UINT16 hdr_offset) {
    hcisu_h2_cb.port_state = HCISU_H2_CLOSED;
    hcisu_h2_cb.rcv_state[0] = HCISU_H2_RX_IDLE;
    hcisu_h2_cb.rcv_state[1] = HCISU_H2_RX_IDLE;
    hcisu_h2_cb.rcv_state[2] = HCISU_H2_RX_IDLE;
    hcisu_h2_cb.task_id = task_id;
    hcisu_h2_cb.mbox = mbox;
    hcisu_h2_cb.hdr_offset = hdr_offset;
}

BOOLEAN hcisu_h2_open(tHCI_CFG* p_cfg) {
    tUUSB uusb;

    uusb.flags = 0;
    uusb.vid = p_cfg->vid;
    uusb.pid = p_cfg->pid;
    uusb.task_id = hcisu_h2_cb.task_id;
    hcisu_h2_cb.port_state = HCISU_H2_OPEN;

    UUSB_Register(&uusb);
    UUSB_Open(&uusb, hcisu_h2_usb_cback);

    return TRUE;
}

void hcisu_h2_close(void) {
    hcisu_h2_cb.port_state = HCISU_H2_CLOSED;
    UUSB_Close();
    UUSB_Unregister();
}

BOOLEAN hcisu_h2_send(HC_BT_HDR* p_msg) {
    hcisu_h2_send_msg(&hcisu_h2_cb, p_msg);
    return TRUE;
}

UINT32 hcisu_h2_handle_event(UINT16 len) {
    hcisu_h2_receive_msg((UINT16)(len - sizeof(HC_BT_HDR) - hcisu_h2_cb.hdr_offset), &hcisu_h2_cb);
    return 0;
}
