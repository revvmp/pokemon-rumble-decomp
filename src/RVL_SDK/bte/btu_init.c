#include <revolution/bte/btm_int.h>
#include <revolution/bte/btu.h>
#include <revolution/bte/hcidefs.h>
#include <revolution/bte/l2c_int.h>
#include <revolution/bte/sdp_int.h>

const BD_ADDR BT_BD_ANY = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void btu_init_core(void) {
    btm_init();
    l2c_init();
    sdp_init();

    btm_discovery_db_init();
}

void BTE_Init(void) {
    memset(&btu_cb, 0, sizeof(btu_cb));

    btu_cb.hcit_acl_pkt_size = BTU_DEFAULT_DATA_SIZE + HCI_DATA_PREAMBLE_SIZE;
    btu_cb.controller_cmd_window = 1;
}
