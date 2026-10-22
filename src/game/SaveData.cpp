#include <game/SaveData.h>

#include <game/FileList.h>
#include <game/Memory.h>
#include <game/Message.h>
#include <game/NandAsync.h>
#include <game/StringFormat.h>
#include <game/SystemMessage.h>
#include <NIntl/basic_formatter.h>
#include <memory>
#include <revolution/net.h>
#include <revolution/tpl.h>
#include <stddef.h>
#include <string.h>

enum SaveMode {
    SAVE_MODE_NORMAL,
    SAVE_MODE_REWRITE,
    SAVE_MODE_NO_BACKUP,
    SAVE_MODE_WRITE_ONLY,
    SAVE_MODE_COMMIT_ONLY
};

enum BannerMode {
    BANNER_MODE_NORMAL,
    BANNER_MODE_WRITE_ONLY,
    BANNER_MODE_COMMIT_ONLY
};

static const int BANNER_ICON_SPEED_END = 0;
static const int BANNER_ICON_SPEED_FAST = 1;

static const u32 SAVE_FILE_SIZE = 0x34000;
static const u32 NAND_REQUIRED_BLOCKS = 30;
static const u32 NAND_REQUIRED_INODES = 5;
static const u32 NAND_CHECK_BLOCKS_MASK = 5;
static const u32 NAND_CHECK_INODES_MASK = 0xA;
static const u16 SAVE_FILE_VERSION = 4;

struct SaveFileHeader {
    u8 mDigest[NET_SHA1_DIGEST_SIZE];
    u16 mVersion;
    u16 mDataSize;
};

VectorStructType SD_PiiPersonalData::sVectorTypes[1] = {VECTOR_STRUCT_NONE};
VectorStructType SD_BattleRoyalData::sVectorTypes[1] = {VECTOR_STRUCT_NONE};
VectorStructType SD_StatisticValue::sVectorTypes[1] = {VECTOR_STRUCT_NONE};
VectorStructType SD_PiiCollectionData::sVectorTypes[1] = {VECTOR_STRUCT_NONE};
VectorStructType UsedPassword::sVectorTypes[1] = {VECTOR_STRUCT_NONE};
VectorStructType SlotData::sVectorTypes[8] = {
    VECTOR_STRUCT_PII_PERSONAL_DATA,  VECTOR_STRUCT_BATTLE_ROYAL_DATA,   VECTOR_STRUCT_STATISTIC_VALUE_1,
    VECTOR_STRUCT_STATISTIC_VALUE_2,  VECTOR_STRUCT_STATISTIC_VALUE_3,   VECTOR_STRUCT_PII_COLLECTION_DATA,
    VECTOR_STRUCT_USED_PASSWORD,      VECTOR_STRUCT_NONE,
};
VectorStructType SaveData::sVectorTypes[2] = {VECTOR_STRUCT_SLOT_DATA, VECTOR_STRUCT_NONE};

static std::tr1::shared_ptr<SaveData> sSaveData;

static inline void ReportNandError(s32 result) {
    switch (result) {
    case NAND_RESULT_CORRUPT:
        ShowSystemMessage(GetMessageText(0x1001D, gEmptyText), false);
        break;
    case NAND_RESULT_FATAL_ERROR:
    case NAND_RESULT_MAXFILES:
    case NAND_RESULT_MAXBLOCKS:
    case NAND_RESULT_INVALID:
    case NAND_RESULT_BUSY:
    case NAND_RESULT_ALLOC_FAILED:
        ShowSystemMessage(GetMessageText(0x1001F, gEmptyText), true);
        break;
    case NAND_RESULT_UNKNOWN:
        ShowSystemMessage(GetMessageText(0x10020, gEmptyText), false);
        break;
    }
}

static inline void WriteU8(u8* pBuffer, u32 offset, u8 value) {
    if (pBuffer != nullptr) {
        pBuffer[offset] = value;
    }
}

static inline void WriteU16(u8* pBuffer, u32 offset, u16 value) {
    if (pBuffer != nullptr) {
        *reinterpret_cast<u16*>(pBuffer + offset) = value;
    }
}

template <typename T> u32 SerializeStruct(T& rStruct, u32 size, u8* pBuffer, u32 offset, u8* pDigests);

template <typename T> u32 SerializeVector(std::vector<T>& rVector, u8* pBuffer, u32 offset, u8* pDigests) {
    WriteU16(pBuffer, offset, sizeof(typename T::Data));
    WriteU16(pBuffer, offset + 2, rVector.size());
    offset += 4;

    u32 digestIndex = 0;
    u8* pDigest;
    for (T* it = rVector.begin().base(); it != rVector.end().base(); ++it) {
        u32 start = offset;
        offset = SerializeStruct(*it, sizeof(typename T::Data), pBuffer, offset, nullptr);
        if (pDigests != nullptr) {
            NETSHA1Context context;
            pDigest = &pDigests[digestIndex++ * NET_SHA1_DIGEST_SIZE];
            NETSHA1Init(&context);
            NETSHA1Update(&context, pBuffer + start, offset - start);
            NETSHA1GetDigest(&context, pDigest);
        }
    }
    return offset;
}

template <typename T> u32 SerializeStruct(T& rStruct, u32 size, u8* pBuffer, u32 offset, u8* pDigests) {
    if (pBuffer != nullptr) {
        memcpy(pBuffer + offset, &rStruct.mData, size);
    }
    offset += size;

    for (int i = 0; i < static_cast<int>(sizeof(T::sVectorTypes) / sizeof(T::sVectorTypes[0])); i++) {
        WriteU8(pBuffer, offset, T::sVectorTypes[i]);
        offset++;

        switch (T::sVectorTypes[i]) {
        case VECTOR_STRUCT_BATTLE_ROYAL_DATA:
            offset = SerializeVector(*rStruct.GetBattleRoyalData(), pBuffer, offset, nullptr);
            break;
        case VECTOR_STRUCT_PII_PERSONAL_DATA:
            offset = SerializeVector(*rStruct.GetPiiPersonalData(), pBuffer, offset, nullptr);
            break;
        case VECTOR_STRUCT_PII_COLLECTION_DATA:
            offset = SerializeVector(*rStruct.GetPiiCollectionData(), pBuffer, offset, nullptr);
            break;
        case VECTOR_STRUCT_SLOT_DATA:
            offset = SerializeVector(*rStruct.GetSlotData(), pBuffer, offset, pDigests);
            break;
        case VECTOR_STRUCT_STATISTIC_VALUE_1:
            offset = SerializeVector(*rStruct.GetStatisticValue1(), pBuffer, offset, nullptr);
            break;
        case VECTOR_STRUCT_STATISTIC_VALUE_2:
            offset = SerializeVector(*rStruct.GetStatisticValue2(), pBuffer, offset, nullptr);
            break;
        case VECTOR_STRUCT_STATISTIC_VALUE_3:
            offset = SerializeVector(*rStruct.GetStatisticValue3(), pBuffer, offset, nullptr);
            break;
        case VECTOR_STRUCT_USED_PASSWORD:
            offset = SerializeVector(*rStruct.GetUsedPassword(), pBuffer, offset, nullptr);
            break;
        }
    }
    return offset;
}

static inline void SetBannerIconSpeed(NANDBanner* pBanner, int index, int speed) {
    pBanner->iconSpeed = (speed << (index * 2)) | (pBanner->iconSpeed & ~(3 << (index * 2)));
}

static inline TPLPalette* GetPalette(const std::tr1::shared_ptr<FileList>& rFiles) {
    return static_cast<TPLPalette*>(rFiles->front()->GetData());
}

static inline void InitBanner(NANDBanner* pBanner, const u16* pTitle, const u16* pSubtitle) {
    NANDInitBanner(pBanner, NAND_BANNER_FLAG_NOTCOPY, pTitle, pSubtitle);
}

bool WriteBanner(int mode) {
    if (mode != BANNER_MODE_COMMIT_ONLY) {
        s32 result = WaitNandCreate("/tmp/banner.bin", NAND_PERM_RUSR | NAND_PERM_WUSR, 0);
        if (result != NAND_RESULT_OK && result != NAND_RESULT_EXISTS) {
            ReportNandError(result);
            return false;
        }

        NANDFileInfo info;
        result = WaitNandOpen("/tmp/banner.bin", &info, NAND_ACCESS_READ | NAND_ACCESS_WRITE);
        if (result != NAND_RESULT_OK) {
            ReportNandError(result);
            return false;
        }

        std::tr1::shared_ptr<NANDBanner> banner(static_cast<NANDBanner*>(GameMemory::Allocate(sizeof(NANDBanner), 64)),
                                                GameMemory::Free);
        InitBanner(banner.get(), reinterpret_cast<const u16*>(GetMessageText(1, gEmptyText)),
                   reinterpret_cast<const u16*>(GetMessageText(0x600D3, gEmptyText)));

        std::list<std::tr1::shared_ptr<FileList> > fileLists;
        std::tr1::shared_ptr<FileList> files = LoadFileList(nullptr, "savebanner.arc/arc/timg/banner.tpl", 0, true, nullptr);
        if (!files) {
            return false;
        }
        fileLists.push_back(files);

        TPLPalette* pPalette = GetPalette(files);
        TPLBind(pPalette);
        memcpy(banner->bannerTexture, TPLGet(pPalette, 0)->textureHeader->data, sizeof(banner->bannerTexture));

        int icon;
        for (icon = 0;; icon++) {
            files = LoadResourceFileList(nullptr, FormatString("savebanner.arc/arc/timg/icon%02d.tpl", icon + 1).c_str(), 0,
                                         true, nullptr);
            if (!files) {
                break;
            }
            fileLists.push_back(files);

            pPalette = GetPalette(files);
            TPLBind(pPalette);
            memcpy(banner->iconTexture[icon], TPLGet(pPalette, 0)->textureHeader->data, sizeof(banner->iconTexture[0]));
            SetBannerIconSpeed(banner.get(), icon, BANNER_ICON_SPEED_FAST);
        }
        SetBannerIconSpeed(banner.get(), icon, BANNER_ICON_SPEED_END);
        fileLists.clear();

        u32 size = offsetof(NANDBanner, iconTexture) + icon * sizeof(banner->iconTexture[0]);
        u32 written = WaitNandWrite(&info, banner.get(), size);
        if (written != size) {
            WaitNandClose(&info);
            ReportNandError(NAND_RESULT_UNKNOWN);
            return false;
        }

        result = WaitNandClose(&info);
        if (result != NAND_RESULT_OK) {
            ReportNandError(result);
            return false;
        }
    }

    if (mode == BANNER_MODE_WRITE_ONLY) {
        return true;
    }

    s32 result = WaitNandMove("/tmp/banner.bin", "./");
    if (result != NAND_RESULT_OK) {
        ReportNandError(result);
    }
    return true;
}

static inline void Scramble(u8* pData, u32 size) {
    for (int blockOffset = 0x10, shift = 1; blockOffset < size; blockOffset += 0x10, shift++) {
        u8* pBlock = pData + blockOffset;
        u8* pDst = pBlock;
        for (int i = 0; i < 0x10 && blockOffset + i < size; i++) {
            *pDst++ += pBlock[(shift + i) % 0x10 - 0x10];
        }
    }
}

static inline s32 MoveToBackup(const char* pPath) {
    s32 result = WaitNandCreateDir("backup/", NAND_PERM_RUSR | NAND_PERM_WUSR | NAND_PERM_RGRP | NAND_PERM_WGRP, 0);
    if (result == NAND_RESULT_OK || result == NAND_RESULT_EXISTS) {
        result = WaitNandMove(pPath, "backup/");
    }
    return result;
}

bool Save(int mode) {
    if (mode != SAVE_MODE_COMMIT_ONLY) {
        SerializeStruct(*sSaveData, sizeof(SaveData::Data), nullptr, 0, nullptr);

        std::tr1::shared_ptr<u8> file(static_cast<u8*>(GameMemory::AllocateMEM2(SAVE_FILE_SIZE, 64)), GameMemory::Free);
        memset(file.get(), 0, SAVE_FILE_SIZE);

        SaveFileHeader* pHeader = reinterpret_cast<SaveFileHeader*>(file.get());
        SerializeStruct(*sSaveData, sizeof(SaveData::Data), reinterpret_cast<u8*>(pHeader + 1), 0,
                        reinterpret_cast<u8*>(sSaveData->mData.mSlotDigests));
        pHeader->mVersion = SAVE_FILE_VERSION;
        pHeader->mDataSize = sizeof(SaveData::Data);

        NETSHA1Context context;
        NETSHA1Init(&context);
        NETSHA1Update(&context, &pHeader->mVersion, SAVE_FILE_SIZE - NET_SHA1_DIGEST_SIZE);
        NETSHA1GetDigest(&context, pHeader->mDigest);

        int shift;
        u8* pDst;
        u8* pBlock;
        u8* pData = file.get();
        u32 blockOffset;
        for (blockOffset = 0x10, shift = 1; blockOffset < SAVE_FILE_SIZE; shift++, blockOffset += 0x10) {
            pBlock = pData + blockOffset;
            pDst = pBlock;
            for (int i = 0; i < 0x10 && blockOffset + i < SAVE_FILE_SIZE; i++) {
                *pDst++ += pBlock[(shift + i) % 0x10 - 0x10];
            }
        }

        s32 result = WaitNandCreate("/tmp/savedata.bin", NAND_PERM_RUSR | NAND_PERM_WUSR | NAND_PERM_RGRP | NAND_PERM_WGRP, 0);
        if (result != NAND_RESULT_OK && result != NAND_RESULT_EXISTS) {
            ReportNandError(result);
            return false;
        }

        NANDFileInfo info;
        result = WaitNandOpen("/tmp/savedata.bin", &info, NAND_ACCESS_READ | NAND_ACCESS_WRITE);
        if (result != NAND_RESULT_OK) {
            ReportNandError(result);
            return false;
        }

        if (WaitNandWrite(&info, file.get(), SAVE_FILE_SIZE) != SAVE_FILE_SIZE) {
            WaitNandClose(&info);
            ReportNandError(NAND_RESULT_UNKNOWN);
            return false;
        }

        result = WaitNandClose(&info);
        if (result != NAND_RESULT_OK) {
            ReportNandError(result);
            return false;
        }

        WriteBanner(BANNER_MODE_WRITE_ONLY);
    }

    if (mode == SAVE_MODE_WRITE_ONLY) {
        return true;
    }

    bool retry = false;
    if (mode != SAVE_MODE_NO_BACKUP) {
        s32 result = MoveToBackup("savedata.bin");
        if (result == NAND_RESULT_NOEXISTS && mode == SAVE_MODE_NORMAL) {
            retry = true;
        } else if (result != NAND_RESULT_OK) {
            ReportNandError(result);
            return false;
        }
    }

    s32 result = WaitNandMove("/tmp/savedata.bin", "./");
    if (result != NAND_RESULT_OK) {
        ReportNandError(result);
        return false;
    }

    if (retry) {
        Save(SAVE_MODE_REWRITE);
    }
    if (mode == SAVE_MODE_REWRITE) {
        return true;
    }
    WriteBanner(BANNER_MODE_COMMIT_ONLY);
    return true;
}

enum LoadResult {
    LOAD_RESULT_OK,
    LOAD_RESULT_NO_FILE = 0x65,
    LOAD_RESULT_EMPTY,
    LOAD_RESULT_CORRUPT,
    LOAD_RESULT_TOO_SMALL,
    LOAD_RESULT_VERSION
};

static const int SLOT_COUNT = 3;

template <typename T> const u8* DeserializeStruct(T& rStruct, u32 size, const u8* pData);

template <typename T> const u8* DeserializeVector(std::vector<T>& rVector, const u8* pData) {
    u16 count = reinterpret_cast<const u16*>(pData)[1];
    u16 size = reinterpret_cast<const u16*>(pData)[0];
    pData += 4;

    rVector.resize(count);
    for (typename std::vector<T>::iterator it = rVector.begin(); it != rVector.end(); ++it) {
        pData = DeserializeStruct(*it, size, pData);
    }
    return pData;
}

template <typename T> const u8* DeserializeStruct(T& rStruct, u32 size, const u8* pData) {
    if (size < T::GetDataSize()) {
        rStruct.ClearData();
    }
    memcpy(&rStruct.mData, pData, size);
    pData += size;

    for (;;) {
        switch (*pData++) {
        case VECTOR_STRUCT_BATTLE_ROYAL_DATA:
            pData = DeserializeVector(*rStruct.GetBattleRoyalData(), pData);
            break;
        case VECTOR_STRUCT_PII_PERSONAL_DATA:
            pData = DeserializeVector(*rStruct.GetPiiPersonalData(), pData);
            break;
        case VECTOR_STRUCT_PII_COLLECTION_DATA:
            pData = DeserializeVector(*rStruct.GetPiiCollectionData(), pData);
            break;
        case VECTOR_STRUCT_SLOT_DATA:
            pData = DeserializeVector(*rStruct.GetSlotData(), pData);
            break;
        case VECTOR_STRUCT_STATISTIC_VALUE_1:
            pData = DeserializeVector(*rStruct.GetStatisticValue1(), pData);
            break;
        case VECTOR_STRUCT_STATISTIC_VALUE_2:
            pData = DeserializeVector(*rStruct.GetStatisticValue2(), pData);
            break;
        case VECTOR_STRUCT_STATISTIC_VALUE_3:
            pData = DeserializeVector(*rStruct.GetStatisticValue3(), pData);
            break;
        case VECTOR_STRUCT_USED_PASSWORD:
            pData = DeserializeVector(*rStruct.GetUsedPassword(), pData);
            break;
        case VECTOR_STRUCT_NONE:
            return pData;
        }
    }
}

static inline void Unscramble(u8* pData, int size) {
    int blockOffset = (size - 1) / 0x10 * 0x10;
    int shift = blockOffset / 0x10;
    for (; blockOffset > 0; shift--, blockOffset -= 0x10) {
        u8* pBlock = pData + blockOffset;
        u8* pDst = pBlock;
        for (int i = 0; i < 0x10 && blockOffset + i < size; i++) {
            *pDst++ -= pBlock[(shift + i) % 0x10 - 0x10];
        }
    }
}

int Load(bool backup) {
    sSaveData.reset(new SaveData);
    sSaveData->mSlotData.resize(SLOT_COUNT);

    std::string path = std::string("nand:") + (backup ? "backup/savedata.bin" : "savedata.bin");
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile(path.c_str(), 0x10000, true);
    if (!file || !file->IsLoaded()) {
        return LOAD_RESULT_NO_FILE;
    }

    int size = file->GetSize();
    if (size == 0) {
        return LOAD_RESULT_EMPTY;
    }
    if (static_cast<u32>(size) < sizeof(SaveFileHeader)) {
        return LOAD_RESULT_TOO_SMALL;
    }

    u8* pData = static_cast<u8*>(file->GetData());
    Unscramble(pData, size);

    SaveFileHeader* pHeader = reinterpret_cast<SaveFileHeader*>(pData);
    NETSHA1Context context;
    u8 digest[NET_SHA1_DIGEST_SIZE];
    NETSHA1Init(&context);
    NETSHA1Update(&context, &pHeader->mVersion, size - NET_SHA1_DIGEST_SIZE);
    NETSHA1GetDigest(&context, digest);
    if (memcmp(digest, pHeader->mDigest, NET_SHA1_DIGEST_SIZE) != 0) {
        return LOAD_RESULT_CORRUPT;
    }

    if (pHeader->mVersion != SAVE_FILE_VERSION) {
        return LOAD_RESULT_VERSION;
    }

    DeserializeStruct(*sSaveData, pHeader->mDataSize, reinterpret_cast<u8*>(pHeader + 1));
    return LOAD_RESULT_OK;
}

void DeleteSaveFiles() {
    WaitNandDelete("banner.bin");
    WaitNandDelete("savedata.bin");
    WaitNandDelete("backup/");
}

bool IsGameFlagSet(int flag) {
    SlotData& rSlot = sSaveData->mSlotData[sSaveData->mData.mCurrentSlot];
    return (reinterpret_cast<u8*>(&rSlot.mData)[flag >> 3] & (1 << (flag & 7))) != 0;
}

void SetFlag(int flag, int value) {
    SlotData& rSlot = sSaveData->mSlotData[sSaveData->mData.mCurrentSlot];
    u8& rByte = reinterpret_cast<u8*>(&rSlot.mData)[flag >> 3];
    int bit = flag & 7;
    rByte = (rByte & ~(1 << bit)) | (value << bit);
}

void ResetSlot(int slot) {
    sSaveData->mSlotData[slot] = SlotData();
}

SlotData* GetSaveData(int slot) {
    if (slot != -1) {
        return &sSaveData->mSlotData[slot];
    }
    return &sSaveData->mSlotData[sSaveData->mData.mCurrentSlot];
}

int GetCurrentSlot() {
    return sSaveData->mData.mCurrentSlot;
}

void SetCurrentSlot(int slot) {
    sSaveData->mData.mCurrentSlot = slot;
}

int GetOption() {
    return sSaveData->mData.mOption;
}

void SetOption(int option) {
    sSaveData->mData.mOption = option;
}

template <typename T> static std::wstring FormatWide(const wchar_t* pFormat, const T& rValue) {
    NIntl::basic_formatter<wchar_t> formatter(pFormat);
    return (formatter % rValue).str();
}

bool CheckNandSpace() {
    u32 blocks;
    u32 inodes;
    s32 result = WaitNandGetHomeDirUsage(&blocks, &inodes);
    if (result != NAND_RESULT_OK) {
        ReportNandError(result);
        return false;
    }

    blocks = NAND_REQUIRED_BLOCKS - blocks;
    inodes = NAND_REQUIRED_INODES - inodes;
    u32 answer;
    result = WaitNandCheck(blocks, inodes, &answer);
    if (result != NAND_RESULT_OK) {
        ReportNandError(result);
        return false;
    }

    if ((answer & NAND_CHECK_BLOCKS_MASK) != 0) {
        u32 neededBlocks = (blocks + 7) >> 3;
        if (neededBlocks > 1) {
            std::wstring message = FormatWide(GetMessageText(0x10018, gEmptyText), neededBlocks);
            ShowSystemMessage(message.c_str(), 2);
        } else {
            ShowSystemMessage(GetMessageText(0x10019, gEmptyText), 2);
        }
        return false;
    }
    if ((answer & NAND_CHECK_INODES_MASK) != 0) {
        ShowSystemMessage(GetMessageText(0x1001A, gEmptyText), 2);
        return false;
    }
    return true;
}
