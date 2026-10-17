#include <nw4r/snd/snd_SoundArchiveFile.h>

#include <string.h>

namespace nw4r {
namespace snd {
namespace detail {

namespace {

const u32 SOUND_INFO_REF_OFFSET_V1_0 = 28;

}

SoundArchiveFileReader::SoundArchiveFileReader()
    : mInfo(nullptr),
      mStringBase(nullptr),
      mStringTable(nullptr),
      mStringTreeSound(nullptr),
      mStringTreePlayer(nullptr),
      mStringTreeGroup(nullptr),
      mStringTreeBank(nullptr) {}

void SoundArchiveFileReader::Init(const void* pSoundArchiveData) {
    if (IsValidFileHeader(pSoundArchiveData)) {
        mHeader =
            *static_cast<const SoundArchiveFile::Header*>(pSoundArchiveData);
    }
}

bool SoundArchiveFileReader::IsValidFileHeader(const void* pSoundArchiveData) {
    const ut::BinaryFileHeader* pFileHeader =
        static_cast<const ut::BinaryFileHeader*>(pSoundArchiveData);

    if (pFileHeader->signature != SoundArchiveFile::SIGNATURE_FILE) {
        return false;
    }

    if (pFileHeader->version < NW4R_VERSION(1, 0)) {
        return false;
    }

    if (pFileHeader->version > SoundArchiveFile::FILE_VERSION) {
        return false;
    }

    return true;
}

void SoundArchiveFileReader::SetStringChunk(const void* pStringChunk,
                                            u32 stringChunkSize) {
    const SoundArchiveFile::SymbolBlock* pSymbolBlock =
        static_cast<const SoundArchiveFile::SymbolBlock*>(pStringChunk);
    const SoundArchiveFile::StringBlock* pStringBlock =
        &pSymbolBlock->stringBlock;

    mStringBase = pStringBlock;

    mStringTable = static_cast<const SoundArchiveFile::StringTable*>(
        GetPtrConst(mStringBase, pStringBlock->stringChunk.tableOffset));

    mStringTreeSound = static_cast<const SoundArchiveFile::StringTree*>(
        GetPtrConst(mStringBase, pStringBlock->stringChunk.soundTreeOffset));

    mStringTreePlayer = static_cast<const SoundArchiveFile::StringTree*>(
        GetPtrConst(mStringBase, pStringBlock->stringChunk.playerTreeOffset));

    mStringTreeGroup = static_cast<const SoundArchiveFile::StringTree*>(
        GetPtrConst(mStringBase, pStringBlock->stringChunk.groupTreeOffset));

    mStringTreeBank = static_cast<const SoundArchiveFile::StringTree*>(
        GetPtrConst(mStringBase, pStringBlock->stringChunk.bankTreeOffset));
}

void SoundArchiveFileReader::SetInfoChunk(const void* pInfoChunk,
                                          u32 infoChunkSize) {
    const SoundArchiveFile::InfoBlock* pInfoBlock =
        static_cast<const SoundArchiveFile::InfoBlock*>(pInfoChunk);

    mInfo = &pInfoBlock->info;
}

SoundArchive::SoundType SoundArchiveFileReader::GetSoundType(u32 soundId) const {
    SoundArchiveFile::SoundType soundType;

    const SoundArchiveFile::SoundCommonInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->soundTableRef, mInfo);

    if (pTable == nullptr) {
        return SoundArchive::SOUND_TYPE_INVALID;
    }

    if (soundId >= pTable->count) {
        return SoundArchive::SOUND_TYPE_INVALID;
    }

    if (GetVersion() >= NW4R_VERSION(1, 1)) {
        const SoundArchiveFile::SoundCommonInfo* pCommonInfo =
            Util::GetDataRefAddress0(pTable->items[soundId], mInfo);

        if (pCommonInfo == nullptr) {
            return SoundArchive::SOUND_TYPE_INVALID;
        }

        soundType =
            static_cast<SoundArchiveFile::SoundType>(pCommonInfo->soundType);
    } else {
        soundType = static_cast<SoundArchiveFile::SoundType>(
            pTable->items[soundId].dataType);
    }

    switch (soundType) {
    case SoundArchiveFile::SOUND_TYPE_SEQ:
        return SoundArchive::SOUND_TYPE_SEQ;

    case SoundArchiveFile::SOUND_TYPE_STRM:
        return SoundArchive::SOUND_TYPE_STRM;

    case SoundArchiveFile::SOUND_TYPE_WAVE:
        return SoundArchive::SOUND_TYPE_WAVE;

    default:
        return SoundArchive::SOUND_TYPE_INVALID;
    }
}

bool SoundArchiveFileReader::ReadSoundInfo(u32 soundId,
                                           SoundArchive::SoundInfo* pInfo) const {
    const SoundArchiveFile::SoundCommonInfo* pSrc = impl_GetSoundInfo(soundId);
    if (pSrc == nullptr) {
        return false;
    }

    pInfo->fileId = pSrc->fileId;
    pInfo->playerId = pSrc->playerId;
    pInfo->actorPlayerId = pSrc->actorPlayerId;
    pInfo->playerPriority = pSrc->playerPriority;
    pInfo->volume = pSrc->volume;
    pInfo->remoteFilter = pSrc->remoteFilter;

    if (GetVersion() >= NW4R_VERSION(1, 2)) {
        pInfo->panMode = static_cast<PanMode>(pSrc->panMode);
        pInfo->panCurve = static_cast<PanCurve>(pSrc->panCurve);
    } else {
        pInfo->panMode = PAN_MODE_BALANCE;
        pInfo->panCurve = PAN_CURVE_SQRT;
    }

    return true;
}

bool SoundArchiveFileReader::ReadSound3DParam(
    u32 soundId, SoundArchive::Sound3DParam* pParam) const {
    const SoundArchiveFile::SoundCommonInfo* pSrc = impl_GetSoundInfo(soundId);
    if (pSrc == nullptr) {
        return false;
    }

    const SoundArchiveFile::Sound3DParam* pSrcParam =
        Util::GetDataRefAddress0(pSrc->param3dRef, mInfo);
    if (pSrcParam == nullptr) {
        return false;
    }

    pParam->flags = pSrcParam->flags;
    pParam->decayCurve = pSrcParam->decayCurve;
    pParam->decayRatio = pSrcParam->decayRatio;
    pParam->spatialParameter = pSrcParam->spatialParameter;

    return true;
}

bool SoundArchiveFileReader::ReadSeqSoundInfo(
    u32 soundId, SoundArchive::SeqSoundInfo* pInfo) const {
    const SoundArchiveFile::SeqSoundInfo* pSrc = impl_GetSeqSoundInfo(soundId);
    if (pSrc == nullptr) {
        return false;
    }

    pInfo->dataOffset = pSrc->dataOffset;
    pInfo->bankId = pSrc->bankId;
    pInfo->channelPriority = pSrc->channelPriority;
    pInfo->allocTrack = pSrc->allocTrack;

    if (GetVersion() >= NW4R_VERSION(1, 3)) {
        pInfo->releasePriorityFixFlag = pSrc->releasePriorityFix;
    } else {
        pInfo->releasePriorityFixFlag = false;
    }

    return true;
}

bool SoundArchiveFileReader::ReadStrmSoundInfo(
    u32 soundId, SoundArchive::StrmSoundInfo* pInfo) const {
    const SoundArchiveFile::StrmSoundInfo* pSrc =
        impl_GetStrmSoundInfo(soundId);
    if (pSrc == nullptr) {
        return false;
    }

    pInfo->startPosition = pSrc->startPosition;
    pInfo->allocTrackFlag = pSrc->allocTrackFlag;

    if (GetVersion() >= NW4R_VERSION(1, 4)) {
        pInfo->allocChannelCount = pSrc->allocChannelCount;
    } else {
        pInfo->allocChannelCount = 0;

        for (u16 trackFlag = pSrc->allocChannelCount; trackFlag != 0;
             trackFlag >>= 1) {
            if (trackFlag & 1) {
                pInfo->allocChannelCount++;
            } else if (trackFlag != 0) {
                return false;
            }
        }
    }

    return true;
}

bool SoundArchiveFileReader::ReadWaveSoundInfo(
    u32 soundId, SoundArchive::WaveSoundInfo* pInfo) const {
    const SoundArchiveFile::WaveSoundInfo* pSrc =
        impl_GetWaveSoundInfo(soundId);
    if (pSrc == nullptr) {
        return false;
    }

    pInfo->subNo = pSrc->subNo;
    pInfo->channelPriority = pSrc->channelPriority;

    if (GetVersion() >= NW4R_VERSION(1, 3)) {
        pInfo->releasePriorityFixFlag = pSrc->releasePriorityFix;
    } else {
        pInfo->releasePriorityFixFlag = false;
    }

    return true;
}

bool SoundArchiveFileReader::ReadBankInfo(u32 bankId,
                                          SoundArchive::BankInfo* pInfo) const {
    const SoundArchiveFile::BankInfo* pSrc = impl_GetBankInfo(bankId);
    if (pSrc == nullptr) {
        return false;
    }

    pInfo->fileId = pSrc->fileId;
    return true;
}

bool SoundArchiveFileReader::ReadPlayerInfo(
    u32 playerId, SoundArchive::PlayerInfo* pInfo) const {
    const SoundArchiveFile::PlayerInfo* pSrc = impl_GetPlayerInfo(playerId);
    if (pSrc == nullptr) {
        return false;
    }

    pInfo->playableSoundCount = pSrc->playableSoundCount;
    pInfo->heapSize = pSrc->heapSize;
    return true;
}

bool SoundArchiveFileReader::ReadGroupInfo(
    u32 groupId, SoundArchive::GroupInfo* pInfo) const {
    const SoundArchiveFile::GroupInfo* pSrc = impl_GetGroupInfo(groupId);
    if (pSrc == nullptr) {
        return false;
    }

    const SoundArchiveFile::GroupItemInfoTable* pItemTable =
        Util::GetDataRefAddress0(pSrc->itemTableRef, mInfo);
    if (pItemTable == nullptr) {
        return false;
    }

    pInfo->extFilePath = Util::GetDataRefAddress0(pSrc->extFilePathRef, mInfo);
    pInfo->offset = pSrc->offset;
    pInfo->size = pSrc->size;
    pInfo->waveDataOffset = pSrc->waveDataOffset;
    pInfo->waveDataSize = pSrc->waveDataSize;
    pInfo->itemCount = pItemTable->count;
    return true;
}

bool SoundArchiveFileReader::ReadGroupItemInfo(
    u32 groupId, u32 index, SoundArchive::GroupItemInfo* pInfo) const {
    const SoundArchiveFile::GroupInfo* pGroupInfo = impl_GetGroupInfo(groupId);
    if (pGroupInfo == nullptr) {
        return false;
    }

    const SoundArchiveFile::GroupItemInfoTable* pItemTable =
        Util::GetDataRefAddress0(pGroupInfo->itemTableRef, mInfo);
    if (pItemTable == nullptr) {
        return false;
    }

    if (index >= pItemTable->count) {
        return false;
    }

    const SoundArchiveFile::GroupItemInfo* pSrc =
        Util::GetDataRefAddress0(pItemTable->items[index], mInfo);
    if (pSrc == nullptr) {
        return false;
    }

    pInfo->fileId = pSrc->fileId;
    pInfo->offset = pSrc->offset;
    pInfo->size = pSrc->size;
    pInfo->waveDataOffset = pSrc->waveDataOffset;
    pInfo->waveDataSize = pSrc->waveDataSize;
    return true;
}

bool SoundArchiveFileReader::ReadSoundArchivePlayerInfo(
    SoundArchive::SoundArchivePlayerInfo* pInfo) const {
    const SoundArchiveFile::SoundArchivePlayerInfo* pSrc =
        Util::GetDataRefAddress0(mInfo->soundArchivePlayerInfoRef, mInfo);
    if (pInfo == nullptr) {
        return false;
    }

    pInfo->seqSoundCount = pSrc->seqSoundCount;
    pInfo->seqTrackCount = pSrc->seqTrackCount;
    pInfo->strmSoundCount = pSrc->strmSoundCount;
    pInfo->strmTrackCount = pSrc->strmTrackCount;
    pInfo->strmChannelCount = pSrc->strmChannelCount;
    pInfo->waveSoundCount = pSrc->waveSoundCount;
    pInfo->waveTrackCount = pSrc->waveTrackCount;
    return true;
}

u32 SoundArchiveFileReader::GetPlayerCount() const {
    const SoundArchiveFile::PlayerInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->playerTableRef, mInfo);
    if (pTable == nullptr) {
        return 0;
    }

    return pTable->count;
}

u32 SoundArchiveFileReader::GetGroupCount() const {
    const SoundArchiveFile::GroupInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->groupTableRef, mInfo);
    if (pTable == nullptr) {
        return 0;
    }

    return pTable->count - 1;
}

u32 SoundArchiveFileReader::GetSoundUserParam(u32 soundId) const {
    const SoundArchiveFile::SoundCommonInfo* pInfo = impl_GetSoundInfo(soundId);
    if (pInfo == nullptr) {
        return 0;
    }

    return pInfo->userParam[0];
}

u32 SoundArchiveFileReader::GetFileCount() const {
    const SoundArchiveFile::FileInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->fileTableRef, mInfo);
    if (pTable == nullptr) {
        return 0;
    }

    return pTable->count;
}

bool SoundArchiveFileReader::ReadFileInfo(u32 fileId,
                                          SoundArchive::FileInfo* pInfo) const {
    const SoundArchiveFile::FileInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->fileTableRef, mInfo);
    if (pTable == nullptr) {
        return false;
    }

    if (fileId >= pTable->count) {
        return false;
    }

    const SoundArchiveFile::FileInfo* pFileInfo =
        Util::GetDataRefAddress0(pTable->items[fileId], mInfo);
    if (pFileInfo == nullptr) {
        return false;
    }

    const SoundArchiveFile::FilePosTable* pPosTable =
        Util::GetDataRefAddress0(pFileInfo->filePosTableRef, mInfo);
    if (pPosTable == nullptr) {
        return false;
    }

    pInfo->fileSize = pFileInfo->fileSize;
    pInfo->waveDataFileSize = pFileInfo->waveDataFileSize;
    pInfo->extFilePath =
        Util::GetDataRefAddress0(pFileInfo->extFilePathRef, mInfo);
    pInfo->filePosCount = pPosTable->count;
    return true;
}

bool SoundArchiveFileReader::ReadFilePos(u32 fileId, u32 index,
                                         SoundArchive::FilePos* pPos) const {
    const SoundArchiveFile::FileInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->fileTableRef, mInfo);
    if (pTable == nullptr) {
        return false;
    }

    if (fileId >= pTable->count) {
        return false;
    }

    const SoundArchiveFile::FileInfo* pFileInfo =
        Util::GetDataRefAddress0(pTable->items[fileId], mInfo);
    if (pFileInfo == nullptr) {
        return false;
    }

    const SoundArchiveFile::FilePosTable* pPosTable =
        Util::GetDataRefAddress0(pFileInfo->filePosTableRef, mInfo);
    if (pPosTable == nullptr) {
        return false;
    }

    if (index >= pPosTable->count) {
        return false;
    }

    const SoundArchive::FilePos* pSrc =
        Util::GetDataRefAddress0(pPosTable->items[index], mInfo);
    if (pSrc == nullptr) {
        return false;
    }

    *pPos = *pSrc;
    return true;
}

const char* SoundArchiveFileReader::GetString(u32 id) const {
    if (id == SoundArchive::INVALID_ID) {
        return nullptr;
    }

    if (mStringTable == nullptr) {
        return nullptr;
    }

    return static_cast<const char*>(
        GetPtrConst(mStringBase, mStringTable->offsetTable.items[id]));
}

u32 SoundArchiveFileReader::ConvertLabelStringToId(
    const SoundArchiveFile::StringTree* pTree, const char* pLabel) const {
    if (pTree == nullptr) {
        return SoundArchive::INVALID_ID;
    }

    if (pTree->rootIdx >= pTree->nodeTable.count) {
        return SoundArchive::INVALID_ID;
    }

    const SoundArchiveFile::StringTreeNode* pNode =
        &pTree->nodeTable.items[pTree->rootIdx];

    u32 length = strlen(pLabel);

    while (!(pNode->flags & 1)) {
        int pos = pNode->bit >> 3;
        int bit = pNode->bit & 7;

        u32 nodeIdx;
        if (pos < static_cast<int>(length) && (1 << (7 - bit)) & pLabel[pos]) {
            nodeIdx = pNode->rightIdx;
        } else {
            nodeIdx = pNode->leftIdx;
        }

        pNode = &pTree->nodeTable.items[nodeIdx];
    }

    if (strcmp(pLabel, GetString(pNode->strIdx)) == 0) {
        return pNode->id;
    }

    return SoundArchive::INVALID_ID;
}

const SoundArchiveFile::SoundCommonInfo*
SoundArchiveFileReader::impl_GetSoundInfo(u32 soundId) const {
    const SoundArchiveFile::SoundCommonInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->soundTableRef, mInfo);

    if (pTable == nullptr) {
        return nullptr;
    }

    if (soundId >= pTable->count) {
        return nullptr;
    }

    if (GetVersion() >= NW4R_VERSION(1, 1)) {
        return Util::GetDataRefAddress0(pTable->items[soundId], mInfo);
    }

    return static_cast<const SoundArchiveFile::SoundCommonInfo*>(
        ut::AddOffsetToPtr(mInfo, pTable->items[soundId].value));
}

bool SoundArchiveFileReader::impl_GetSoundInfoOffset(
    u32 soundId, SoundArchiveFile::SoundInfoRef* pRef) const {
    const SoundArchiveFile::SoundCommonInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->soundTableRef, mInfo);

    if (pTable == nullptr) {
        return false;
    }

    if (soundId >= pTable->count) {
        return false;
    }

    if (GetVersion() >= NW4R_VERSION(1, 1)) {
        const SoundArchiveFile::SoundCommonInfo* pCommonInfo =
            Util::GetDataRefAddress0(pTable->items[soundId], mInfo);

        if (pCommonInfo == nullptr) {
            return false;
        }

        *pRef = pCommonInfo->soundInfoRef;
        return true;
    }

    SoundArchiveFile::SoundInfoRef ref;
    ref.refType = pTable->items[soundId].refType;
    ref.dataType = pTable->items[soundId].dataType;
    ref.value = pTable->items[soundId].value + SOUND_INFO_REF_OFFSET_V1_0;

    *pRef = ref;
    return true;
}

const SoundArchiveFile::SeqSoundInfo*
SoundArchiveFileReader::impl_GetSeqSoundInfo(u32 soundId) const {
    if (GetSoundType(soundId) != SoundArchive::SOUND_TYPE_SEQ) {
        return nullptr;
    }

    SoundArchiveFile::SoundInfoRef ref;
    if (!impl_GetSoundInfoOffset(soundId, &ref)) {
        return nullptr;
    }

    return Util::GetDataRefAddress1(ref, mInfo);
}

const SoundArchiveFile::StrmSoundInfo*
SoundArchiveFileReader::impl_GetStrmSoundInfo(u32 soundId) const {
    if (GetSoundType(soundId) != SoundArchive::SOUND_TYPE_STRM) {
        return nullptr;
    }

    SoundArchiveFile::SoundInfoRef ref;
    if (!impl_GetSoundInfoOffset(soundId, &ref)) {
        return nullptr;
    }

    return Util::GetDataRefAddress2(ref, mInfo);
}

const SoundArchiveFile::WaveSoundInfo*
SoundArchiveFileReader::impl_GetWaveSoundInfo(u32 soundId) const {
    if (GetSoundType(soundId) != SoundArchive::SOUND_TYPE_WAVE) {
        return nullptr;
    }

    SoundArchiveFile::SoundInfoRef ref;
    if (!impl_GetSoundInfoOffset(soundId, &ref)) {
        return nullptr;
    }

    return Util::GetDataRefAddress3(ref, mInfo);
}

const SoundArchiveFile::BankInfo*
SoundArchiveFileReader::impl_GetBankInfo(u32 bankId) const {
    const SoundArchiveFile::BankInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->bankTableRef, mInfo);

    if (pTable == nullptr) {
        return nullptr;
    }

    if (bankId >= pTable->count) {
        return nullptr;
    }

    return Util::GetDataRefAddress0(pTable->items[bankId], mInfo);
}

const SoundArchiveFile::PlayerInfo*
SoundArchiveFileReader::impl_GetPlayerInfo(u32 playerId) const {
    const SoundArchiveFile::PlayerInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->playerTableRef, mInfo);

    if (pTable == nullptr) {
        return nullptr;
    }

    if (playerId >= pTable->count) {
        return nullptr;
    }

    return Util::GetDataRefAddress0(pTable->items[playerId], mInfo);
}

const SoundArchiveFile::GroupInfo*
SoundArchiveFileReader::impl_GetGroupInfo(u32 groupId) const {
    const SoundArchiveFile::GroupInfoTable* pTable =
        Util::GetDataRefAddress0(mInfo->groupTableRef, mInfo);

    if (pTable == nullptr) {
        return nullptr;
    }

    if (groupId >= pTable->count) {
        return nullptr;
    }

    return Util::GetDataRefAddress0(pTable->items[groupId], mInfo);
}

}
}
}
