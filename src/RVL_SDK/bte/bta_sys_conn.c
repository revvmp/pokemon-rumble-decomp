#include <RVL_SDK/bte/sys.h>

void bta_sys_rm_register(tBTA_SYS_CONN_CBACK* p_cback) {
    bta_sys_cb.prm_cb = p_cback;
}

void bta_sys_compress_register(tBTA_SYS_CONN_CBACK* p_cback) {
    bta_sys_cb.compress_cb = p_cback;
}

void bta_sys_pm_register(tBTA_SYS_CONN_CBACK* p_cback) {
    bta_sys_cb.ppm_cb = p_cback;
}

void bta_sys_conn_open(unsigned char id, unsigned char app_id, unsigned char* peer_addr) {
    if (bta_sys_cb.prm_cb)
        (*bta_sys_cb.prm_cb)(BTA_SYS_CONN_OPEN, id, app_id, peer_addr);
    if (bta_sys_cb.ppm_cb)
        (*bta_sys_cb.ppm_cb)(BTA_SYS_CONN_OPEN, id, app_id, peer_addr);
    if (bta_sys_cb.compress_cb)
        (*bta_sys_cb.compress_cb)(BTA_SYS_CONN_OPEN, id, app_id, peer_addr);
}

void bta_sys_conn_close(unsigned char id, unsigned char app_id, unsigned char* peer_addr) {
    if (bta_sys_cb.prm_cb)
        (*bta_sys_cb.prm_cb)(BTA_SYS_CONN_CLOSE, id, app_id, peer_addr);
    if (bta_sys_cb.ppm_cb)
        (*bta_sys_cb.ppm_cb)(BTA_SYS_CONN_CLOSE, id, app_id, peer_addr);
    if (bta_sys_cb.compress_cb)
        (*bta_sys_cb.compress_cb)(BTA_SYS_CONN_CLOSE, id, app_id, peer_addr);
}

void bta_sys_sco_close(unsigned char id, unsigned char app_id, unsigned char* peer_addr) {
    if (bta_sys_cb.ppm_cb)
        (*bta_sys_cb.ppm_cb)(BTA_SYS_SCO_CLOSE, id, app_id, peer_addr);
}

void bta_sys_idle(unsigned char id, unsigned char app_id, unsigned char* peer_addr) {
    if (bta_sys_cb.ppm_cb)
        (*bta_sys_cb.ppm_cb)(BTA_SYS_CONN_IDLE, id, app_id, peer_addr);
}

void bta_sys_busy(unsigned char id, unsigned char app_id, unsigned char* peer_addr) {
    if (bta_sys_cb.ppm_cb)
        (*bta_sys_cb.ppm_cb)(BTA_SYS_CONN_BUSY, id, app_id, peer_addr);
}
