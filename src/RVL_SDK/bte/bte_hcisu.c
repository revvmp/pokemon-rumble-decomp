#include "RVL_SDK/bte/transport.h"

extern void GKI_freebuf(void* p_buffer);
extern void GKI_send_msg(unsigned char task_id, unsigned char mailbox, void* p_msg);

tHCI_IF* p_hcisu_if;
tHCI_CFG* p_hcisu_cfg;

void bte_hcisu_send(HC_BT_HDR* p_msg, unsigned short event) {
    p_msg->event = event;
    if (p_hcisu_if)
        p_hcisu_if->send(p_msg);
    else
        GKI_freebuf(p_msg);
}

void bte_hcisu_task(unsigned long parameter) {
    if (p_hcisu_if) {
        if (p_hcisu_if->init)
            p_hcisu_if->init(2, 1, 0x800);
        if (p_hcisu_if->open)
            p_hcisu_if->open(p_hcisu_cfg);
    }
}

void bte_hcisu_close(void) {
    if (p_hcisu_if) {
        if (p_hcisu_if->close)
            p_hcisu_if->close();
    }
}

void bta_ci_hci_msg_handler(void* p_data) {
    GKI_send_msg(2, 0, p_data);
}
