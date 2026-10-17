#ifndef RVL_SDK_BTE_L2C_API_H
#define RVL_SDK_BTE_L2C_API_H

#include <revolution/bte/bt_types.h>

typedef BOOLEAN tL2CA_COMPRESS_CB(BD_ADDR peer_addr, int, int, int, int, int, int, UINT8** p_mem_pool, UINT32* mem_pool_size);

BOOLEAN L2CA_SetIdleTimeoutByBdAddr(BD_ADDR bd_addr, UINT16 timeout);
void L2CA_RegisterCompression(tL2CA_COMPRESS_CB* p_cb, int);

#endif
