#include <nw4r/snd/snd_SoundArchive.h>

#include <nw4r/snd/snd_SoundArchiveFile.h>

#include <string.h>

namespace nw4r {
namespace snd {

SoundArchive::SoundArchive() : mFileReader(nullptr) {
    mExtFileRoot[0] = '/';
    mExtFileRoot[1] = '\0';
}

SoundArchive::~SoundArchive() {}

bool SoundArchive::IsAvailable() const {
    return mFileReader != nullptr;
}

void SoundArchive::Setup(detail::SoundArchiveFileReader* pFileReader) {
    mFileReader = pFileReader;
}

u32 SoundArchive::GetPlayerCount() const {
    return mFileReader->GetPlayerCount();
}

u32 SoundArchive::GetGroupCount() const {
    return mFileReader->GetGroupCount();
}

u32 SoundArchive::ConvertLabelStringToSoundId(const char* pLabel) const {
    return mFileReader->ConvertLabelStringToSoundId(pLabel);
}

u32 SoundArchive::GetSoundUserParam(u32 soundId) const {
    return mFileReader->GetSoundUserParam(soundId);
}

SoundArchive::SoundType SoundArchive::GetSoundType(u32 soundId) const {
    return mFileReader->GetSoundType(soundId);
}

bool SoundArchive::ReadSoundInfo(u32 soundId, SoundInfo* pInfo) const {
    return mFileReader->ReadSoundInfo(soundId, pInfo);
}

bool SoundArchive::ReadSeqSoundInfo(u32 soundId, SeqSoundInfo* pInfo) const {
    return mFileReader->ReadSeqSoundInfo(soundId, pInfo);
}

bool SoundArchive::detail_ReadStrmSoundInfo(u32 soundId,
                                            StrmSoundInfo* pInfo) const {
    return mFileReader->ReadStrmSoundInfo(soundId, pInfo);
}

bool SoundArchive::detail_ReadWaveSoundInfo(u32 soundId,
                                            WaveSoundInfo* pInfo) const {
    return mFileReader->ReadWaveSoundInfo(soundId, pInfo);
}

bool SoundArchive::ReadPlayerInfo(u32 playerId, PlayerInfo* pInfo) const {
    return mFileReader->ReadPlayerInfo(playerId, pInfo);
}

bool SoundArchive::ReadSoundArchivePlayerInfo(
    SoundArchivePlayerInfo* pInfo) const {
    return mFileReader->ReadSoundArchivePlayerInfo(pInfo);
}

bool SoundArchive::detail_ReadSound3DParam(u32 soundId,
                                           Sound3DParam* pParam) const {
    return mFileReader->ReadSound3DParam(soundId, pParam);
}

bool SoundArchive::ReadBankInfo(u32 bankId, BankInfo* pInfo) const {
    return mFileReader->ReadBankInfo(bankId, pInfo);
}

bool SoundArchive::detail_ReadGroupInfo(u32 groupId, GroupInfo* pInfo) const {
    return mFileReader->ReadGroupInfo(groupId, pInfo);
}

bool SoundArchive::detail_ReadGroupItemInfo(u32 groupId, u32 index,
                                            GroupItemInfo* pInfo) const {
    return mFileReader->ReadGroupItemInfo(groupId, index, pInfo);
}

u32 SoundArchive::detail_GetFileCount() const {
    return mFileReader->GetFileCount();
}

bool SoundArchive::detail_ReadFileInfo(u32 fileId, FileInfo* pInfo) const {
    return mFileReader->ReadFileInfo(fileId, pInfo);
}

bool SoundArchive::detail_ReadFilePos(u32 fileId, u32 index,
                                      FilePos* pPos) const {
    return mFileReader->ReadFilePos(fileId, index, pPos);
}

inline ut::FileStream* SoundArchive::OpenExtStreamImpl(
    void* pBuffer, int size, const char* pExtFilePath, u32 begin,
    u32 length) const {
    const char* pFullPath;
    char pathBuffer[FILE_PATH_MAX + 1];

    if (pExtFilePath[0] == '/') {
        pFullPath = pExtFilePath;
    } else {
        u32 fileLen = strlen(pExtFilePath);
        u32 dirLen = strlen(mExtFileRoot);

        if (fileLen + dirLen >= FILE_PATH_MAX + 1) {
            return nullptr;
        }

        strncpy(pathBuffer, mExtFileRoot, dirLen + 1);
        strncat(pathBuffer, pExtFilePath, fileLen + 1);
        pFullPath = pathBuffer;
    }

    return OpenExtStream(pBuffer, size, pFullPath, begin, length);
}

ut::FileStream* SoundArchive::detail_OpenFileStream(u32 fileId, void* pBuffer,
                                                    int size) const {
    FileInfo fileInfo;
    if (!detail_ReadFileInfo(fileId, &fileInfo)) {
        return nullptr;
    }

    if (fileInfo.extFilePath != nullptr) {
        return OpenExtStreamImpl(pBuffer, size, fileInfo.extFilePath, 0,
                                 fileInfo.fileSize);
    }

    FilePos filePos;
    if (!detail_ReadFilePos(fileId, 0, &filePos)) {
        return nullptr;
    }

    GroupInfo groupInfo;
    if (!detail_ReadGroupInfo(filePos.groupId, &groupInfo)) {
        return nullptr;
    }

    GroupItemInfo itemInfo;
    if (!detail_ReadGroupItemInfo(filePos.groupId, filePos.index, &itemInfo)) {
        return nullptr;
    }

    u32 itemOffset = groupInfo.offset + itemInfo.offset;
    u32 itemSize = itemInfo.size;

    if (groupInfo.extFilePath != nullptr) {
        return OpenExtStreamImpl(pBuffer, size, groupInfo.extFilePath,
                                 itemOffset, itemSize);
    }

    return OpenStream(pBuffer, size, itemOffset, itemSize);
}

}
}
