#ifndef RVL_SDK_CNT_H
#define RVL_SDK_CNT_H

typedef struct CNTHandle CNTHandle;

typedef struct CNTFileInfo {
    union {
        struct {
            CNTHandle* handle;
            unsigned int startOffset;
            unsigned int length;
            int readOffset;
        } nand;
        struct {
            unsigned char commandBlock[0x30];
            unsigned int startOffset;
            unsigned int length;
            void* callback;
            int readOffset;
        } dvd;
    } file;
    unsigned char type;
} CNTFileInfo;

void CNTInit(void);
int CNTShutdown(void);
unsigned int CNTGetLength(const CNTFileInfo* pFileInfo);

#endif
