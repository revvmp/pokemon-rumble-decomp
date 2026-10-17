#ifndef RVL_SDK_BTE_PORT_INT_H
#define RVL_SDK_BTE_PORT_INT_H

#include <revolution/bte/bt_target.h>
#include <revolution/bte/bt_types.h>
#include <revolution/bte/gki.h>
#include <revolution/bte/port_api.h>
#include <revolution/bte/rfcdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    PORT_FC_UNDEFINED = 0,
    PORT_FC_TS710 = 1,
    PORT_FC_CREDIT = 2
};

enum {
    MODEM_SIGNAL_DTRDSR = 1 << 0,
    MODEM_SIGNAL_RTSCTS = 1 << 1,
    MODEM_SIGNAL_RI = 1 << 2,
    MODEM_SIGNAL_DCD = 1 << 3
};

typedef struct {
    TIMER_LIST_ENT tle;
    BUFFER_Q cmd_q;
    UINT8 port_inx[RFCOMM_MAX_DLCI + 1];
    BD_ADDR bd_addr;
    UINT16 lcid;
    UINT16 peer_l2cap_mtu;
    UINT8 state;
    BOOLEAN is_initiator;
    BOOLEAN local_cfg_sent;
    BOOLEAN peer_cfg_rcvd;
    BOOLEAN restart_required;
    BOOLEAN peer_ready;
    UINT8 flow;
    BOOLEAN l2cap_congested;
    BOOLEAN is_disc_initiator;
} tRFC_MCB;

typedef struct {
    UINT8 state;
    UINT8 expected_rsp;
    tRFC_MCB* p_mcb;
    TIMER_LIST_ENT tle;
} tRFC_PORT;

typedef struct {
    BUFFER_Q queue;
    BOOLEAN peer_fc;
    BOOLEAN user_fc;
    UINT32 queue_size;
    tPORT_CALLBACK* p_callback;
} tPORT_DATA;

typedef struct {
    UINT8 modem_signal;
    UINT8 break_signal;
    UINT8 discard_buffers;
    UINT8 break_signal_seq;
    BOOLEAN fc;
} tPORT_CTRL;

typedef struct {
    UINT8 inx;
    BOOLEAN in_use;
    UINT8 state;
    UINT8 scn;
    UINT16 uuid;
    BD_ADDR bd_addr;
    BOOLEAN is_server;
    UINT8 dlci;
    UINT8 error;
    UINT8 line_status;
    UINT8 default_signal_state;
    UINT16 mtu;
    UINT16 peer_mtu;
    tPORT_DATA tx;
    tPORT_DATA rx;
    tPORT_STATE user_port_pars;
    tPORT_STATE peer_port_pars;
    tPORT_CTRL local_ctrl;
    tPORT_CTRL peer_ctrl;
    UINT8 port_ctrl;
    BOOLEAN rx_flag_ev_pending;
    tRFC_PORT rfc;
    UINT32 ev_mask;
    tPORT_CALLBACK* p_callback;
    tPORT_CALLBACK* p_mgmt_callback;
    tPORT_DATA_CALLBACK* p_data_callback;
    UINT16 credit_tx;
    UINT16 credit_rx;
    UINT16 credit_rx_max;
    UINT16 credit_rx_low;
    UINT16 rx_buf_critical;
} tPORT;

typedef struct {
    tPORT port[MAX_RFC_PORTS];
    tRFC_MCB rfc_mcb[MAX_BD_CONNECTIONS];
} tPORT_CB;

void PORT_FlowInd(tRFC_MCB* p_mcb, UINT8 dlci, BOOLEAN enable_data);
void port_rfc_closed(tPORT* p_port, UINT8 res);

#ifdef __cplusplus
}
#endif

#endif
