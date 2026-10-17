#ifndef NW4R_SND_SOUND_ARCHIVE_FILE_H
#define NW4R_SND_SOUND_ARCHIVE_FILE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_algorithm.h>

#include <nw4r/snd/snd_SoundArchive.h>
#include <nw4r/snd/snd_Util.h>

#include <nw4r/ut/ut_binaryFileFormat.h>

namespace nw4r {
namespace snd {
namespace detail {

namespace SoundArchiveFile {

struct Header {
    ut::BinaryFileHeader fileHeader;
    u32 symbolDataOffset;
    u32 symbolDataSize;
    u32 infoOffset;
    u32 infoSize;
    u32 fileImageOffset;
    u32 fileImageSize;
};

struct StringTreeNode {
    u16 flags;
    u16 bit;
    u32 leftIdx;
    u32 rightIdx;
    u32 strIdx;
    u32 id;
};

struct StringTree {
    u32 rootIdx;
    Util::Table<StringTreeNode> nodeTable;
};

struct StringTable {
    Util::Table<u32> offsetTable;
};

struct StringChunk {
    u32 tableOffset;
    u32 soundTreeOffset;
    u32 playerTreeOffset;
    u32 groupTreeOffset;
    u32 bankTreeOffset;
};

union StringBlock {
    StringTable stringTable;
    StringChunk stringChunk;
};

struct SymbolBlock {
    ut::BinaryBlockHeader blockHeader;
    StringBlock stringBlock;
};

struct Sound3DParam {
    u32 flags;
    u8 decayCurve;
    u8 decayRatio;
    u8 dopplerFactor;
};

struct SeqSoundInfo {
    u32 dataOffset;
    u32 bankId;
    u32 allocTrack;
    u8 channelPriority;
    u8 releasePriorityFix;
    u8 padding[2];
    u32 reserved;
};

struct StrmSoundInfo {
    u32 startPosition;
    u16 allocChannelCount;
    u16 allocTrackFlag;
    u32 reserved;
};

struct WaveSoundInfo {
    s32 subNo;
    u32 allocTrack;
    u8 channelPriority;
    u8 releasePriorityFix;
    u8 padding[2];
    u32 reserved;
};

typedef Util::DataRef<void, SeqSoundInfo, StrmSoundInfo, WaveSoundInfo>
    SoundInfoRef;

struct SoundCommonInfo {
    u32 stringId;
    u32 fileId;
    u32 playerId;
    Util::DataRef<Sound3DParam> param3dRef;
    u8 volume;
    u8 playerPriority;
    u8 soundType;
    u8 remoteFilter;
    SoundInfoRef soundInfoRef;
    u32 userParam[2];
    u8 panMode;
    u8 panCurve;
    u8 actorPlayerId;
    u8 reserved;
};

typedef Util::Table<Util::DataRef<SoundCommonInfo> > SoundCommonInfoTable;

struct SoundArchivePlayerInfo {
    u16 seqSoundCount;
    u16 seqTrackCount;
    u16 strmSoundCount;
    u16 strmTrackCount;
    u16 strmChannelCount;
    u16 waveSoundCount;
    u16 waveTrackCount;
    u16 padding;
    u32 reserved;
};

struct BankInfo {
    u32 stringId;
    u32 fileId;
    u32 reserved;
};

typedef Util::Table<Util::DataRef<BankInfo> > BankInfoTable;

struct PlayerInfo {
    u32 stringId;
    u8 playableSoundCount;
    u8 padding;
    u16 padding2;
    u32 heapSize;
    u32 reserved;
};

typedef Util::Table<Util::DataRef<PlayerInfo> > PlayerInfoTable;

struct GroupItemInfo {
    u32 fileId;
    u32 offset;
    u32 size;
    u32 waveDataOffset;
    u32 waveDataSize;
    u32 reserved;
};

typedef Util::Table<Util::DataRef<GroupItemInfo> > GroupItemInfoTable;

struct GroupInfo {
    u32 stringId;
    s32 entryNum;
    Util::DataRef<char> extFilePathRef;
    u32 offset;
    u32 size;
    u32 waveDataOffset;
    u32 waveDataSize;
    Util::DataRef<GroupItemInfoTable> itemTableRef;
};

typedef Util::Table<Util::DataRef<GroupInfo> > GroupInfoTable;

typedef Util::Table<Util::DataRef<SoundArchive::FilePos> > FilePosTable;

struct FileInfo {
    u32 fileSize;
    u32 waveDataFileSize;
    s32 entryNum;
    Util::DataRef<char> extFilePathRef;
    Util::DataRef<FilePosTable> filePosTableRef;
};

typedef Util::Table<Util::DataRef<FileInfo> > FileInfoTable;

struct Info {
    Util::DataRef<SoundCommonInfoTable> soundTableRef;
    Util::DataRef<BankInfoTable> bankTableRef;
    Util::DataRef<PlayerInfoTable> playerTableRef;
    Util::DataRef<FileInfoTable> fileTableRef;
    Util::DataRef<GroupInfoTable> groupTableRef;
    Util::DataRef<SoundArchivePlayerInfo> soundArchivePlayerInfoRef;
};

struct InfoBlock {
    ut::BinaryBlockHeader blockHeader;
    Info info;
};

static const u32 SIGNATURE_FILE = FOURCC('R', 'S', 'A', 'R');
static const u32 SIGNATURE_SYMB_BLOCK = FOURCC('S', 'Y', 'M', 'B');
static const u32 SIGNATURE_INFO_BLOCK = FOURCC('I', 'N', 'F', 'O');
static const int FILE_VERSION = NW4R_VERSION(1, 4);

}

class SoundArchiveFileReader {
public:
    static const u16 SUPPORTED_FILE_VERSION = NW4R_VERSION(1, 4);

public:
    SoundArchiveFileReader();

    void Init(const void* pSoundArchiveData);

    u16 GetVersion() const {
        return mHeader.fileHeader.version;
    }
    u32 GetLabelStringChunkOffset() const {
        return mHeader.symbolDataOffset;
    }
    u32 GetLabelStringChunkSize() const {
        return mHeader.symbolDataSize;
    }
    u32 GetInfoChunkOffset() const {
        return mHeader.infoOffset;
    }
    u32 GetInfoChunkSize() const {
        return mHeader.infoSize;
    }

    const char* GetString(u32 id) const;
    SoundArchive::SoundType GetSoundType(u32 soundId) const;
    u32 GetSoundStringId(u32 soundId) const;
    u32 GetBankCount() const;
    u32 GetPlayerCount() const;
    u32 GetGroupCount() const;
    u32 GetFileCount() const;

    const void* GetPtrConst(const void* pBase, u32 offset) const {
        if (offset == 0) {
            return nullptr;
        }

        return ut::AddOffsetToPtr(pBase, offset);
    }

    void SetStringChunk(const void* pStringChunk, u32 size);
    void SetInfoChunk(const void* pInfoChunk, u32 size);

    bool ReadSeqSoundInfo(u32 soundId, SoundArchive::SeqSoundInfo* pInfo) const;
    bool ReadStrmSoundInfo(u32 soundId,
                           SoundArchive::StrmSoundInfo* pInfo) const;
    bool ReadWaveSoundInfo(u32 soundId,
                           SoundArchive::WaveSoundInfo* pInfo) const;
    bool ReadSoundArchivePlayerInfo(
        SoundArchive::SoundArchivePlayerInfo* pInfo) const;
    bool ReadSoundInfo(u32 soundId, SoundArchive::SoundInfo* pInfo) const;

    bool ReadBankInfo(u32 bankId, SoundArchive::BankInfo* pInfo) const;

    bool ReadPlayerInfo(u32 playerId, SoundArchive::PlayerInfo* pInfo) const;
    bool ReadSound3DParam(u32 soundId,
                          SoundArchive::Sound3DParam* pParam) const;

    bool ReadGroupItemInfo(u32 groupId, u32 index,
                           SoundArchive::GroupItemInfo* pInfo) const;
    bool ReadGroupInfo(u32 groupId, SoundArchive::GroupInfo* pInfo) const;

    bool ReadFileInfo(u32 fileId, SoundArchive::FileInfo* pInfo) const;
    bool ReadFilePos(u32 fileId, u32 index, SoundArchive::FilePos* pPos) const;

    u32 ConvertLabelStringToId(const SoundArchiveFile::StringTree* pTree,
                               const char* pLabel) const;
    const char* GetSoundLabelString(u32 soundId) const;
    u32 GetSoundUserParam(u32 soundId) const;

    u32 ConvertLabelStringToSoundId(const char* pLabel) const {
        return ConvertLabelStringToId(mStringTreeSound, pLabel);
    }
    u32 ConvertLabelStringToPlayerId(const char* pLabel) const {
        return ConvertLabelStringToId(mStringTreePlayer, pLabel);
    }
    u32 ConvertLabelStringToGroupId(const char* pLabel) const {
        return ConvertLabelStringToId(mStringTreeGroup, pLabel);
    }
    u32 ConvertLabelStringToBankId(const char* pLabel) const {
        return ConvertLabelStringToId(mStringTreeBank, pLabel);
    }

private:
    static bool IsValidFileHeader(const void* pSoundArchiveData);

    const SoundArchiveFile::SeqSoundInfo* impl_GetSeqSoundInfo(
        u32 soundId) const;
    const SoundArchiveFile::StrmSoundInfo* impl_GetStrmSoundInfo(
        u32 soundId) const;
    const SoundArchiveFile::WaveSoundInfo* impl_GetWaveSoundInfo(
        u32 soundId) const;
    const SoundArchiveFile::SoundCommonInfo* impl_GetSoundInfo(
        u32 soundId) const;

    bool impl_GetSoundInfoOffset(
        u32 soundId, SoundArchiveFile::SoundInfoRef* pSoundInfoRef) const;

    const SoundArchiveFile::BankInfo* impl_GetBankInfo(u32 bankId) const;
    const SoundArchiveFile::PlayerInfo* impl_GetPlayerInfo(u32 playerId) const;
    const SoundArchiveFile::GroupInfo* impl_GetGroupInfo(u32 groupId) const;

private:
    SoundArchiveFile::Header mHeader;
    const SoundArchiveFile::Info* mInfo;
    const void* mStringBase;
    const SoundArchiveFile::StringTable* mStringTable;
    const SoundArchiveFile::StringTree* mStringTreeSound;
    const SoundArchiveFile::StringTree* mStringTreePlayer;
    const SoundArchiveFile::StringTree* mStringTreeGroup;
    const SoundArchiveFile::StringTree* mStringTreeBank;
};

}
}
}

#endif
