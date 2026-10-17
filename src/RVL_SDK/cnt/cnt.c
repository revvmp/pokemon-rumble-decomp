#include <RVL_SDK/cnt.h>

extern int ESP_InitLib(void);
extern int ESP_CloseLib(void);
extern int OSDisableInterrupts(void);
extern int OSRestoreInterrupts(int enabled);
extern void OSRegisterVersion(const char* version);

static int sCntInitialized;
static const char* sCntVersion = "<< RVL_SDK - CNT \trelease build: Feb 27 2009 10:05:46 (0x4302_145) >>";

void CNTInit(void) {
    if (!sCntInitialized) {
        int enabled;
        ESP_InitLib();
        enabled = OSDisableInterrupts();
        sCntInitialized = 1;
        OSRestoreInterrupts(enabled);
        OSRegisterVersion(sCntVersion);
    }
}

int CNTShutdown(void) {
    return ESP_CloseLib();
}

unsigned int CNTGetLength(const CNTFileInfo* pFileInfo) {
    switch (pFileInfo->type) {
    case 1:
        return pFileInfo->file.nand.length;
    case 2:
        return pFileInfo->file.dvd.length;
    default:
        return 0;
    }
}
