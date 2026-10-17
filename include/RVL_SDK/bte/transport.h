#ifndef RVL_SDK_BTE_TRANSPORT_H
#define RVL_SDK_BTE_TRANSPORT_H

typedef struct {
    unsigned short event;
    unsigned short len;
    unsigned short offset;
    unsigned short layer_specific;
} HC_BT_HDR;

typedef struct tHCI_CFG tHCI_CFG;

typedef struct {
    void (*init)(unsigned char, unsigned char, unsigned short);
    unsigned char (*open)(tHCI_CFG*);
    void (*close)(void);
    unsigned char (*send)(HC_BT_HDR*);
    unsigned long (*handle_event)(unsigned short);
} tHCI_IF;

extern tHCI_IF* p_hcisu_if;
extern tHCI_CFG* p_hcisu_cfg;
void bte_hcisu_send(HC_BT_HDR* p_msg, unsigned short event);
void bte_hcisu_task(unsigned long parameter);
void bte_hcisu_close(void);
void bta_ci_hci_msg_handler(void* p_data);

#endif
