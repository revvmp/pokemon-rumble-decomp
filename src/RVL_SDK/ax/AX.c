#include "revolution/ax/AX.h"

extern void OSRegisterVersion(const char*);
extern void __AXAllocInit(void);
extern void __AXVPBInit(void);
extern void __AXSPBInit(void);
extern void __AXAuxInit(void);
extern void __AXClInit(void);
extern void __AXOutInit(unsigned int);

const char* __AXVersion = "<< RVL_SDK - AX \trelease build: Feb 27 2009 10:01:36 (0x4302_145) >>";
static int __AXInitialized = 0;

void AXInit(void) {
    if (!__AXInitialized) {
        OSRegisterVersion(__AXVersion);
        __AXAllocInit();
        __AXVPBInit();
        __AXSPBInit();
        __AXAuxInit();
        __AXClInit();
        __AXOutInit(0);
        __AXInitialized = 1;
    }
}

int AXIsInit(void) {
    return __AXInitialized;
}
