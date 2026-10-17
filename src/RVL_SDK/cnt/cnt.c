#include <RVL_SDK/cnt.h>
#include <RVL_SDK/esp/esp.h>

extern void MEMFreeToAllocator(void* pAllocator, void* pBlock);
extern int DVDClose(CNTFileInfo* pFileInfo);
extern int DVDReadPrio(CNTFileInfo* pFileInfo, void* pBuffer, int length, int offset, int priority);
extern int contentConvertErrorCode(int result);
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

int CNTReleaseHandle(CNTHandle* pHandle) {
    switch (pHandle->type) {
    case 1:
        MEMFreeToAllocator(pHandle->content.nand.allocator, pHandle->content.nand.archiveStart);
        return contentConvertErrorCode(ESP_CloseContentFile(pHandle->content.nand.descriptor));
    case 2:
        return 0;
    default:
        return -5063;
    }
}

int CNTReadAt(CNTFileInfo* pFileInfo, void* pBuffer, int length, int offset) {
    int result;
    int position;
    switch (pFileInfo->type) {
    case 1:
        position = pFileInfo->file.nand.readOffset + offset;
        if (position < 0 || position > pFileInfo->file.nand.length) {
            return -5009;
        }
        result = ESP_SeekContentFile(pFileInfo->file.nand.handle->content.nand.descriptor,
                                 pFileInfo->file.nand.startOffset + position, 0);
        if (result < 0) {
            return contentConvertErrorCode(result);
        }
        return contentConvertErrorCode(ESP_ReadContentFile(pFileInfo->file.nand.handle->content.nand.descriptor,
                                                       pBuffer, length));
    case 2:
        result = DVDReadPrio(pFileInfo, pBuffer, length, offset + pFileInfo->file.dvd.readOffset, 2);
        if (result < 0) {
            switch (result) {
            case -3:
                result = -5014;
                break;
            default:
                result = -5003;
                break;
            }
        }
        return result;
    default:
        return -5063;
    }
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

int CNTClose(CNTFileInfo* pFileInfo) {
    switch (pFileInfo->type) {
    case 1:
        return 0;
    case 2:
        return DVDClose(pFileInfo) ? 0 : -5005;
    default:
        return -5063;
    }
}
