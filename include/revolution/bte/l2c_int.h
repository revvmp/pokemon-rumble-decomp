#ifndef RVL_SDK_BTE_L2C_INT_H
#define RVL_SDK_BTE_L2C_INT_H

#include <revolution/bte/l2c_api.h>

void l2c_init(void);
BOOLEAN l2c_link_hci_conn_comp(UINT8 status, UINT16 handle, BD_ADDR p_bda);
BOOLEAN l2c_link_hci_disc_comp(UINT16 handle, UINT8 reason);
BOOLEAN l2c_link_hci_qos_violation(UINT16 handle);
void l2c_link_process_num_completed_pkts(UINT8* p);
void l2c_link_role_changed(BD_ADDR bd_addr, UINT8 new_role);
void l2c_link_role_change_failed(void);
void l2c_pin_code_request(BD_ADDR bd_addr);

#endif
