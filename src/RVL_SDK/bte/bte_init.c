extern void RFCOMM_Init(void);
extern void GAP_Init(void);
extern void HID_DevInit(void);
extern void HID_HostInit(void);

void BTE_InitStack(void) {
    RFCOMM_Init();
    GAP_Init();
    HID_DevInit();
    HID_HostInit();
}
