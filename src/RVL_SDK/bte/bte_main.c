#include "revolution/bte/transport.h"
#include "revolution/types.h"
#include "revolution/OS/OSAlarm.h"

typedef void (*BTAAppInfoCallback)(unsigned char status);

unsigned char __BTUInterruptHandlerStack[0x1000] __attribute__((aligned(32)));
OSAlarm _bte_alarm __attribute__((aligned(8)));
BTAAppInfoCallback _bte_app_info;
volatile unsigned char bte_target_mode;
extern const tHCI_IF hcisu_h2;
unsigned short bte_hcisu_h2_cfg[2] = {0x0A5C, 0x2101};
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

void bta_usb_close_evt(signed char status) {
    if (_bte_app_info) {
        if (status >= 0)
            _bte_app_info(0);
        else
            _bte_app_info(1);
    }
}
