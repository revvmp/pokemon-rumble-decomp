#ifndef RVL_SDK_BTE_L2C_API_H
#define RVL_SDK_BTE_L2C_API_H

#include <revolution/bte/bt_types.h>

typedef BOOLEAN tL2CA_COMPRESS_CB(BD_ADDR peer_addr, int, int, int, int, int, int, UINT8** p_mem_pool, UINT32* mem_pool_size);

typedef struct {
    UINT8 mode;
    UINT8 tx_win_sz;
    UINT8 max_transmit;
    UINT16 rtrans_tout;
    UINT16 mon_tout;
    UINT16 mps;
    UINT8 tx_pool_id;
    UINT16 tx_buf_sz;
    UINT8 rx_pool_id;
} tL2CAP_FCR_OPTS;

typedef struct {
    UINT16 result;
    BOOLEAN mtu_present;
    UINT16 mtu;
    BOOLEAN qos_present;
    FLOW_SPEC qos;
    BOOLEAN flush_to_present;
    UINT16 flush_to;
    BOOLEAN fcr_present;
    tL2CAP_FCR_OPTS fcr;
    UINT16 cong_param;
    UINT16 flags;
} tL2CAP_CFG_INFO;

UINT16 L2CA_ConnectReq(UINT16 psm, BD_ADDR p_bd_addr);
BOOLEAN L2CA_SetIdleTimeoutByBdAddr(BD_ADDR bd_addr, UINT16 timeout);
void L2CA_RegisterCompression(tL2CA_COMPRESS_CB* p_cb, int);

#endif
