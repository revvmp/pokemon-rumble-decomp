#include "revolution/bte/transport.h"
#include "revolution/types.h"

typedef struct OSAlarm OSAlarm;
typedef struct OSContext OSContext;
typedef void (*BTAAppInfoCallback)(unsigned char status);

extern unsigned char __BTUInterruptHandlerStack[0x1000];
extern OSAlarm _bte_alarm;
extern BTAAppInfoCallback _bte_app_info;
extern const tHCI_IF hcisu_h2;
extern unsigned short bte_hcisu_h2_cfg[2];
extern volatile int wait4hci;

extern void OSSwitchFiber(unsigned long entry, unsigned long stack);
extern void OSCreateAlarm(OSAlarm* alarm);
extern long long OSGetTime(void);
extern void OSSetPeriodicAlarm(OSAlarm* alarm, long long start, long long period, void (*handler)(OSAlarm*, OSContext*));
extern void OSCancelAlarm(OSAlarm* alarm);
extern void GKI_init(void);
extern void GKI_enable(void);
extern void GKI_run(void* parameter);
extern void GKI_delay(unsigned long milliseconds);
extern void GKI_shutdown(void);
extern void BTE_Init(void);
extern unsigned char BTA_DmIsDeviceUp(void);
extern void btu_task_init(void);
extern void btu_task_msg_handler(void);

void BTUInterruptHandler(OSAlarm* alarm, OSContext* context) {
    OSSwitchFiber((unsigned long)btu_task_msg_handler, (unsigned long)__BTUInterruptHandlerStack + sizeof __BTUInterruptHandlerStack);
}

unsigned char BTA_Init(void) {
    _bte_app_info = NULL;
    GKI_init();
    GKI_enable();
    BTE_Init();
    p_hcisu_if = (tHCI_IF*)&hcisu_h2;
    p_hcisu_cfg = (tHCI_CFG*)&bte_hcisu_h2_cfg;
    bte_hcisu_task(0);
    while (wait4hci)
        GKI_delay(100);
    btu_task_init();
    OSCreateAlarm(&_bte_alarm);
    OSSetPeriodicAlarm(&_bte_alarm, OSGetTime(), ((*(volatile unsigned long*)0x800000F8 / 4) / 1000) * 2, BTUInterruptHandler);
    GKI_run(NULL);
    while (!BTA_DmIsDeviceUp())
        GKI_delay(2000);
    return 0;
}

void BTA_CleanUp(BTAAppInfoCallback callback) {
    _bte_app_info = callback;
    OSCancelAlarm(&_bte_alarm);
    bte_hcisu_close();
    GKI_shutdown();
}
