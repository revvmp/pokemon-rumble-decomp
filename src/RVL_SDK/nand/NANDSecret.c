#include <revolution/nand.h>
#include <revolution/fs.h>
#include <revolution/os/OSThread.h>
#include <revolution/os/OSReset.h>
#include <revolution/os/OSTime.h>
#include <revolution/os.h>

s32 NANDSecretGetUsage(const char* path, u32* fsBlock, u32* inode) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }
    else {
        char absPath[64] = "";
        nandGenerateAbsPath(absPath, path);
        return nandConvertErrorCode(ISFS_GetUsage((const u8*)absPath, fsBlock, inode));
    }
}
