#ifndef RVL_SDK_BTE_PORT_API_H
#define RVL_SDK_BTE_PORT_API_H

#include <revolution/bte/bt_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_ERR_GROUP 0

#define PORT_XON_DC1 0x11
#define PORT_XOFF_DC3 0x13

enum {
    PORT_BAUD_RATE_2400 = 0,
    PORT_BAUD_RATE_4800 = 1,
    PORT_BAUD_RATE_7200 = 2,
    PORT_BAUD_RATE_9600 = 3
};

enum {
    PORT_5_BITS = 0,
    PORT_6_BITS = 1,
    PORT_7_BITS = 2,
    PORT_8_BITS = 3
};

enum {
    PORT_ONESTOPBIT = 0,
    PORT_ONE5STOPBITS = 1
};

enum {
    PORT_PARITY_NO = 0,
    PORT_PARITY_YES = 1
};

enum {
    PORT_ODD_PARITY = 0,
    PORT_EVEN_PARITY = 1,
    PORT_MARK_PARITY = 2,
    PORT_SPACE_PARITY = 3
};

enum {
    PORT_FC_OFF = 0
};

enum {
    PORT_EV_RXCHAR = 1 << 0,
    PORT_EV_RXFLAG = 1 << 1,
    PORT_EV_TXEMPTY = 1 << 2,
    PORT_EV_CTS = 1 << 3,
    PORT_EV_DSR = 1 << 4,
    PORT_EV_RLSD = 1 << 5,
    PORT_EV_BREAK = 1 << 6,
    PORT_EV_ERR = 1 << 7,
    PORT_EV_RING = 1 << 8,
    PORT_EV_CONNECTED = 1 << 9,
    PORT_EV_CTSS = 1 << 10,
    PORT_EV_DSRS = 1 << 11,
    PORT_EV_RLSDS = 1 << 12,
    PORT_EV_OVERRUN = 1 << 13,
    PORT_EV_TXCHAR = 1 << 14,
    PORT_EV_CONNECT_ERR = 1 << 15,
    PORT_EV_FC = 1 << 16,
    PORT_EV_FCS = 1 << 17
};

enum {
    PORT_DTRDSR_ON = 1 << 0,
    PORT_CTSRTS_ON = 1 << 1,
    PORT_RING_ON = 1 << 2,
    PORT_DCD_ON = 1 << 3
};

enum {
    PORT_SUCCESS = PORT_ERR_GROUP + 0,
    PORT_UNKNOWN_ERROR = PORT_ERR_GROUP + 1,
    PORT_ALREADY_OPENED = PORT_ERR_GROUP + 2,
    PORT_CMD_PENDING = PORT_ERR_GROUP + 3,
    PORT_NO_MEM = PORT_ERR_GROUP + 5,
    PORT_NO_RESOURCES = PORT_ERR_GROUP + 6,
    PORT_BAD_HANDLE = PORT_ERR_GROUP + 9,
    PORT_NOT_OPENED = PORT_ERR_GROUP + 10,
    PORT_LINE_ERR = PORT_ERR_GROUP + 11,
    PORT_START_FAILED = PORT_ERR_GROUP + 12,
    PORT_PAR_NEG_FAILED = PORT_ERR_GROUP + 13,
    PORT_PORT_NEG_FAILED = PORT_ERR_GROUP + 14,
    PORT_SEC_FAILED = PORT_ERR_GROUP + 15,
    PORT_PEER_CONNECTION_FAILED = PORT_ERR_GROUP + 16,
    PORT_PEER_TIMEOUT = PORT_ERR_GROUP + 18,
    PORT_CLOSED = PORT_ERR_GROUP + 19,
    PORT_TX_FULL = PORT_ERR_GROUP + 20,
    PORT_TX_QUEUE_DISABLED = PORT_ERR_GROUP + 23,
    PORT_PAGE_TIMEOUT = PORT_ERR_GROUP + 24,
    PORT_INVALID_SCN = PORT_ERR_GROUP + 25
};

typedef void tPORT_CALLBACK(UINT32 code, UINT16 port_handle);
typedef int tPORT_DATA_CALLBACK(UINT16 port_handle, void* p_data, UINT16 len);

typedef struct {
    UINT8 baud_rate;
    UINT8 byte_size;
    UINT8 stop_bits;
    UINT8 parity;
    UINT8 parity_type;
    UINT8 fc_type;
    UINT8 rx_char1;
    UINT8 xon_char;
    UINT8 xoff_char;
} tPORT_STATE;

#ifdef __cplusplus
}
#endif

#endif
