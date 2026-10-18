#ifndef REVOLUTION_FA_H
#define REVOLUTION_FA_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PF_FILE FAFile;
typedef struct PF_DTA FADta;
typedef struct PF_DRV_TBL FADrvTbl;

typedef struct FANandInfo {
    s8 nandFilePath[78];
    u32 nandFileSize;
} FANandInfo;

typedef struct FACommandBlock {
    void* userData;
    u32 asyncData[70];
    FANandInfo nandInfo;
} FACommandBlock;

typedef void (*FACallback)(s32 result, FACommandBlock* block);

s32 FAAttachAsync(u8 device, s8* nand_path, u32 nand_size, FADrvTbl* p_driver_table, FACallback callback, FACommandBlock* block);
s32 FAMountAsync(s8 drive, FACallback callback, FACommandBlock* block);
s32 FAUnmountAsync(s8 drive, u32 mode, FACallback callback, FACommandBlock* block);
s32 FACreateAsync(const char* path, s32 mode, FAFile** pp_file, FACallback callback, FACommandBlock* block);
s32 FAFwriteAsync(void* buffer, u32 size, u32 count, FAFile* p_file, u32* p_written, FACallback callback, FACommandBlock* block);
s32 FAFsfirstAsync(const char* path, u8 attributes, FADta* p_dta, FACallback callback, FACommandBlock* block);
s32 FAFsnextAsync(FADta* p_dta, FACallback callback, FACommandBlock* block);
s32 FARemoveAsync(const char* path, FACallback callback, FACommandBlock* block);
s32 FAMkdirAsync(const char* path, FACallback callback, FACommandBlock* block);
s32 FARmdirAsync(const char* path, FACallback callback, FACommandBlock* block);

#ifdef __cplusplus
}
#endif

#endif
