#ifndef REVOLUTION_FA_H
#define REVOLUTION_FA_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PF_FILE FAFile;
typedef struct PF_DIR FADir;
typedef struct PF_VOLUME FAVolume;
typedef struct FADta {
    FAFile* pFile;
    FADir* pDir;
    FAVolume* pVol;
    u32 parentStartCluster;
    u32 parentPos;
    u32 status;
    u8 numEntryLFNs;
    u8 ordinal;
    u8 checkSum;
    u8 attr;
    s8 regExp[520];
    u16 Time;
    u16 Date;
    u32 FileSize;
    u8 Attribute;
    s8 FileName[13];
    s8 LongName[522];
} FADta;
typedef struct PF_DRV_TBL FADrvTbl;
struct PF_DEV_INF;

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
typedef void (*FAMediaCallback)(s8 drive);

s32 FAInit(long config);
s32 FARegistCB(u8 device, FAMediaCallback inserted, FAMediaCallback ejected);
void FANotifySDInsert(s8 drive);
void FANotifySDEject(s8 drive);
void FANotifyUSBInsert(s8 drive);
void FANotifyUSBEject(s8 drive);
s32 FABuffering(s8 drive, s32 mode);
s32 FADetach(s8 drive);
s32 FAFclose(FAFile* p_file);
s32 FAGetdev(s8 drive, struct PF_DEV_INF* p_info);
s32 FAIsWriteProtected(s8 drive);
s32 FARemove(const char* path);
s32 FASetupFSI(s8 drive, s16 flags);
s32 FAUnmount(s8 drive, u32 mode);
s32 FAChdir(const char* pPath);
s32 FAErrnum(void);

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
