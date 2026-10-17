#ifndef RVL_SDK_BTE_L2C_API_H
#define RVL_SDK_BTE_L2C_API_H

#include <revolution/bte/bt_types.h>

#define L2CAP_MIN_OFFSET 9

enum {
    L2CAP_CONN_OK = 0,
    L2CAP_CONN_PENDING = 1,
    L2CAP_CONN_NO_PSM = 2,
    L2CAP_CONN_SECURITY_BLOCK = 3,
    L2CAP_CONN_NO_RESOURCES = 4
};

enum {
    L2CAP_CFG_OK = 0,
    L2CAP_CFG_UNACCEPTABLE_PARAMS = 1,
    L2CAP_CFG_UNKNOWN_OPTIONS = 3
};

enum {
    L2CAP_PING_RESULT_OK = 0,
    L2CAP_PING_RESULT_NO_LINK = 1,
    L2CAP_PING_RESULT_NO_RESP = 2
};

typedef void tL2CA_ECHO_RSP_CB(UINT16 result);

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

typedef void tL2CA_CONNECT_IND_CB(BD_ADDR bd_addr, UINT16 lcid, UINT16 psm, UINT8 id);
typedef void tL2CA_CONNECT_CFM_CB(UINT16 lcid, UINT16 result);
typedef void tL2CA_CONNECT_PND_CB(UINT16 lcid);
typedef void tL2CA_CONFIG_IND_CB(UINT16 lcid, tL2CAP_CFG_INFO* p_cfg);
typedef void tL2CA_CONFIG_CFM_CB(UINT16 lcid, tL2CAP_CFG_INFO* p_cfg);
typedef void tL2CA_DISCONNECT_IND_CB(UINT16 lcid, BOOLEAN is_conf_needed);
typedef void tL2CA_DISCONNECT_CFM_CB(UINT16 lcid, UINT16 result);
typedef void tL2CA_QOS_VIOLATION_IND_CB(BD_ADDR bd_addr);
typedef void tL2CA_DATA_IND_CB(UINT16 lcid, BT_HDR* p_buf);
typedef void tL2CA_CONGESTION_STATUS_CB(UINT16 lcid, BOOLEAN is_congested);

typedef struct {
    tL2CA_CONNECT_IND_CB* pL2CA_ConnectInd_Cb;
    tL2CA_CONNECT_CFM_CB* pL2CA_ConnectCfm_Cb;
    tL2CA_CONNECT_PND_CB* pL2CA_ConnectPnd_Cb;
    tL2CA_CONFIG_IND_CB* pL2CA_ConfigInd_Cb;
    tL2CA_CONFIG_CFM_CB* pL2CA_ConfigCfm_Cb;
    tL2CA_DISCONNECT_IND_CB* pL2CA_DisconnectInd_Cb;
    tL2CA_DISCONNECT_CFM_CB* pL2CA_DisconnectCfm_Cb;
    tL2CA_QOS_VIOLATION_IND_CB* pL2CA_QoSViolationInd_Cb;
    tL2CA_DATA_IND_CB* pL2CA_DataInd_Cb;
    tL2CA_CONGESTION_STATUS_CB* pL2CA_CongestionStatus_Cb;
} tL2CAP_APPL_INFO;

typedef BOOLEAN tL2CA_COMPRESS_CB(BD_ADDR peer_addr, int, int, int, int, int, int, UINT8** p_mem_pool, UINT32* mem_pool_size);

BOOLEAN L2CA_Register(UINT16 psm, tL2CAP_APPL_INFO* p_cb_info);
UINT16 L2CA_ConnectReq(UINT16 psm, BD_ADDR p_bd_addr);
void L2CA_Deregister(UINT16 psm);
BOOLEAN L2CA_ConnectRsp(BD_ADDR p_bd_addr, UINT8 id, UINT16 lcid, UINT16 result, UINT16 status);
BOOLEAN L2CA_ConfigReq(UINT16 cid, tL2CAP_CFG_INFO* p_cfg);
BOOLEAN L2CA_ConfigRsp(UINT16 cid, tL2CAP_CFG_INFO* p_cfg);
BOOLEAN L2CA_DisconnectReq(UINT16 cid);
BOOLEAN L2CA_DisconnectRsp(UINT16 cid);
UINT8 L2CA_DataWrite(UINT16 cid, BT_HDR* p_data);
BOOLEAN L2CA_SetIdleTimeoutByBdAddr(BD_ADDR bd_addr, UINT16 timeout);
void L2CA_RegisterCompression(tL2CA_COMPRESS_CB* p_cb, int);

BOOLEAN L2CA_SetIdleTimeout(UINT16 cid, UINT16 timeout, BOOLEAN is_global);
#endif
