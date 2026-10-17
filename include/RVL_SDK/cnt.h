#ifndef RVL_SDK_CNT_H
#define RVL_SDK_CNT_H

typedef struct CNTHandle {
    union {
        struct {
            void* archiveStart;
            unsigned char archiveMetadata[24];
            int descriptor;
            void* allocator;
        } nand;
        struct {
            unsigned int index;
            int rootDirectory;
            int currentDirectory;
            unsigned char reserved[24];
        } dvd;
    } content;
    unsigned char type;
} CNTHandle;

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
int CNTReleaseHandle(CNTHandle* pHandle);
int CNTReadAt(CNTFileInfo* pFileInfo, void* pBuffer, int length, int offset);
int CNTClose(CNTFileInfo* pFileInfo);
unsigned int CNTGetLength(const CNTFileInfo* pFileInfo);

#endif
