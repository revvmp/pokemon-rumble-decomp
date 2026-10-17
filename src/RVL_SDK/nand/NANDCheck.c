#include <revolution/nand.h>
#include <revolution/fs.h>
#include <revolution/os/OSThread.h>
#include <revolution/os/OSReset.h>
#include <revolution/os/OSTime.h>
#include <revolution/os.h>
#define NAND_MAX_HOME_FSBLOCKS_MB 16
#define NAND_MAX_HOME_INODES 32
#define NAND_MAX_USER_FSBLOCKS_MB 272
#define NAND_MAX_USER_INODES 4000
#define NAND_MAX_USER_FSBLOCKS 17408
#define NAND_MAX_HOME_FSBLOCKS 1024
#define NAND_MB_TO_FSBLOCKS(x) ((x) * 64)
#define NAND_CHECK_SUCCESS 0
#define NAND_CHECK_HOME_INSSPACE 1
#define NAND_CHECK_HOME_INSINODE 2
#define NAND_CHECK_SYS_INSSPACE 4
#define NAND_CHECK_SYS_INSINODE 8
#define NAND_STATE_CHECK_USAGE 20
#define NAND_STATE_AVAILABLE_AREA 21



static const char* USER_DIR_LIST[] = {
    "/meta",
    "/ticket",
    "/title/00010000",
    "/title/00010001",
    "/title/00010003",
    "/title/00010004",
    "/title/00010005",
    "/title/00010006",
    "/title/00010007",
    "/shared2/title",
    NULL
};

static u32 nandSubtract(u32 num0, u32 num1) {
    return num0 < num1 ? 0 : num0 - num1;
}

static u32 nandCheck(u32 requiredSpace, u32 requiredInodes, u32 usedHomeSpace, u32 usedHomeInodes, u32 usedUserSpace, u32 usedUserInodes) {
    static const u32 HOME_SPACE_LIMIT = NAND_MAX_HOME_FSBLOCKS_MB;
    static const u32 HOME_INODE_LIMIT = NAND_MAX_HOME_INODES;

    static const u32 USER_SPACE_LIMIT = NAND_MAX_USER_FSBLOCKS_MB;
    static const u32 USER_INODE_LIMIT = NAND_MAX_USER_INODES;

    u32 answer = NAND_CHECK_SUCCESS;

    if ((usedHomeSpace + requiredSpace) > NAND_MB_TO_FSBLOCKS(HOME_SPACE_LIMIT)) {
        answer |= NAND_CHECK_HOME_INSSPACE;
    }
    if ((usedHomeInodes + requiredInodes) > (HOME_INODE_LIMIT + 1)) {
        answer |= NAND_CHECK_HOME_INSINODE;
    }
    if ((usedUserSpace + requiredSpace) > NAND_MB_TO_FSBLOCKS(USER_SPACE_LIMIT)) {
        answer |= NAND_CHECK_SYS_INSSPACE;
    }
    if ((usedUserInodes + requiredInodes) > USER_INODE_LIMIT) {
        answer |= NAND_CHECK_SYS_INSINODE;
    }

    return answer;
}

static void nandUserAreaCallback(s32 result, void* arg);
s32 NANDCheckAsync(u32 requiredSpace, u32 requiredInodes, u32* answer, NANDCallback callback, NANDCommandBlock* block) {
    s32 isfsErr = ISFS_ERROR_UNKNOWN;

    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    block->reqBlocks = requiredSpace;
    block->reqInodes = requiredInodes;
    block->answer = answer;
    block->userBlocks = 0;
    block->userInodes = 0;
    block->workBlocks = 0;
    block->workInodes = 0;
    block->dir = USER_DIR_LIST;
    block->state = NAND_STATE_CHECK_USAGE;

    isfsErr = ISFS_GetUsageAsync((const u8*)nandGetHomeDir(), &block->homeBlocks, &block->homeInodes, nandUserAreaCallback, block);
    return nandConvertErrorCode(isfsErr);
}

static void nandUserAreaCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result == ISFS_ERROR_OK || result == ISFS_ERROR_NOEXISTS) {
        const char* dir = *block->dir;
        if (result == ISFS_ERROR_OK) {
            block->userBlocks += block->workBlocks;
            block->userInodes += block->workInodes;
        }

        if (dir) {
            s32 isfsErr = ISFS_ERROR_UNKNOWN;

            block->dir++;

            isfsErr = ISFS_GetUsageAsync((const u8*)dir, &block->workBlocks, &block->workInodes, nandUserAreaCallback, arg);
            if (isfsErr != ISFS_ERROR_OK) {
                ((NANDCallback)block->callback)(nandConvertErrorCode(isfsErr), block);
            }
        } else if (block->state == NAND_STATE_CHECK_USAGE) {
            *block->answer =
                nandCheck(block->reqBlocks, block->reqInodes, block->homeBlocks, block->homeInodes, block->userBlocks, block->userInodes);

            ((NANDCallback)block->callback)(nandConvertErrorCode(ISFS_ERROR_OK), block);
        } else if (block->state == NAND_STATE_AVAILABLE_AREA) {
            u32 usedUserSpace = nandSubtract(NAND_MAX_USER_FSBLOCKS, block->userBlocks);
            u32 freeUserInodes = nandSubtract(NAND_MAX_USER_INODES, block->userInodes);
            u32 usedHomeSpace = nandSubtract(NAND_MAX_HOME_FSBLOCKS, block->homeBlocks);
            u32 usedHomeInodes = nandSubtract(NAND_MAX_HOME_INODES + 1, block->homeInodes);

            *(u32*)block->bytes = usedUserSpace < usedHomeSpace ? usedUserSpace : usedHomeSpace;
            *(u32*)block->inodes = freeUserInodes < usedHomeInodes ? freeUserInodes : usedHomeInodes;

            ((NANDCallback)block->callback)(nandConvertErrorCode(ISFS_ERROR_OK), block);
        } else {
            OSReport("Illegal status is detected at %s()", __FUNCTION__);

            ((NANDCallback)block->callback)(nandConvertErrorCode(ISFS_ERROR_UNKNOWN), block);
        }
    } else {
        ((NANDCallback)block->callback)(nandConvertErrorCode(result), block);
    }
}
