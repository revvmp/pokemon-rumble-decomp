#ifndef NW4R_SND_SOUND_ARCHIVE_FILE_H
#define NW4R_SND_SOUND_ARCHIVE_FILE_H

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_SoundArchive.h"
#include "nw4r/snd/snd_Util.h"

#include "nw4r/ut/ut_binaryFileFormat.h"
#include "nw4r/ut/ut_algorithm.h"

namespace nw4r { namespace snd { namespace detail
{
    struct SoundArchiveFile
    {

        struct Header
        {
            ut::BinaryFileHeader fileHeader;
            u32 symbolDataOffset;
            u32 symbolDataSize;
            u32 infoOffset;
            u32 infoSize;
            u32 fileImageOffset;
            u32 fileImageSize;
        };

        static const int HEADER_AREA_SIZE = ((sizeof(Header) + 31) & ~31) + 40;

        struct StringTreeNode
        {
            u16 flags;
            u16 bit;
            u32 leftIdx;
            u32 rightIdx;
            u32 strIdx;
            u32 id;
        };

        struct StringTree
        {
            u32 rootIdx;
            Util::Table<StringTreeNode> nodeTable;
        };

        struct StringTable
        {
            Util::Table<u32> offsetTable;
        };

        struct StringChunk
        {
            u32 tableOffset;
            u32 soundTreeOffset;
            u32 playerTreeOffset;
            u32 groupTreeOffset;
            u32 bankTreeOffset;
        };

        union StringBlock
        {
            StringTable stringTable;
            StringChunk stringChunk;
        };

        static u32 const SIGNATURE_SYMB_BLOCK =
            FOURCC('S', 'Y', 'M', 'B');

        struct SymbolBlock
        {
            ut::BinaryBlockHeader blockHeader;
            StringBlock stringBlock;
        };

        enum SoundType
        {
            SOUND_TYPE_INVALID,

            SOUND_TYPE_SEQ,
            SOUND_TYPE_STRM,
            SOUND_TYPE_WAVE,
        };

        struct Sound3DParam {
            u32 flags;
            u8 decayCurve;
            u8 decayRatio;
            u8 spatialParameter;
        };

        struct SeqSoundInfo
        {
            u32 dataOffset;
            u32 bankId;
            u32 allocTrack;
            u8 channelPriority;
            u8 releasePriorityFix;
            u8 padding[2];
            u32 reserved;
        };

        struct StrmSoundInfo
        {
            u32 startPosition;
            u16 allocChannelCount;
            u16 allocTrackFlag;
            u32 reserved;
        };

        struct WaveSoundInfo
        {
            s32 subNo;
            u32 allocTrack;
            u8 channelPriority;
            u8 releasePriorityFix;
            u8 padding[2];
            u32 reserved;
        };

        typedef Util::DataRef<void, SeqSoundInfo, StrmSoundInfo, WaveSoundInfo>
            SoundInfoRef;

        struct SoundCommonInfo
        {
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

        typedef Util::Table<Util::DataRef<SoundCommonInfo> >
            SoundCommonInfoTable;

        struct SoundArchivePlayerInfo
        {
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

        struct BankInfo
        {
            u32 stringId;
            u32 fileId;
            u32 reserved;
        };

        typedef Util::Table<Util::DataRef<BankInfo> > BankInfoTable;

        struct PlayerInfo
        {
            u32 stringId;
            u8 playableSoundCount;
            u8 padding;
            u16 padding2;
            u32 heapSize;
            u32 reserved;
        };

        typedef Util::Table<Util::DataRef<PlayerInfo> > PlayerInfoTable;

        struct GroupItemInfo
        {
            u32 fileId;
            u32 offset;
            u32 size;
            u32 waveDataOffset;
            u32 waveDataSize;
            u32 reserved;
        };

        typedef Util::Table<Util::DataRef<GroupItemInfo> > GroupItemInfoTable;

        struct GroupInfo
        {
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

        struct FileInfo
        {
            u32 fileSize;
            u32 waveDataFileSize;
            s32 entryNum;
            Util::DataRef<char> extFilePathRef;
            Util::DataRef<FilePosTable> filePosTableRef;
        };

        typedef Util::Table<Util::DataRef<FileInfo> > FileInfoTable;

        struct Info
        {
            Util::DataRef<SoundCommonInfoTable> soundTableRef;
            Util::DataRef<BankInfoTable> bankTableRef;
            Util::DataRef<PlayerInfoTable> playerTableRef;
            Util::DataRef<FileInfoTable> fileTableRef;
            Util::DataRef<GroupInfoTable> groupTableRef;
            Util::DataRef<SoundArchivePlayerInfo> soundArchivePlayerInfoRef;
        };

        static u32 const SIGNATURE_INFO_BLOCK =
            FOURCC('I', 'N', 'F', 'O');

        struct InfoBlock
        {
            ut::BinaryBlockHeader blockHeader;
            Info info;
        };

        static u32 const SIGNATURE_FILE =
            FOURCC('R', 'S', 'A', 'R');
        static int const FILE_VERSION = NW4R_VERSION(1, 4);
    };
}}}

namespace nw4r { namespace snd { namespace detail
{

    class SoundArchiveFileReader
    {

    public:

        SoundArchiveFileReader();
        ~SoundArchiveFileReader() {}

        void Init(void const *soundArchiveData);

        u16 GetVersion() const { return mHeader.fileHeader.version; }
        u32 GetLabelStringChunkOffset() const
        {
            return mHeader.symbolDataOffset;
        }
        u32 GetLabelStringChunkSize() const { return mHeader.symbolDataSize; }
        u32 GetInfoChunkOffset() const { return mHeader.infoOffset; }
        u32 GetInfoChunkSize() const { return mHeader.infoSize; }

        char const *GetString(u32 id) const;
        SoundArchive::SoundType GetSoundType(u32 soundId) const;
        u32 GetSoundStringId(u32 id) const;
        u32 GetBankCount() const;
        u32 GetPlayerCount() const;
        u32 GetGroupCount() const;
        u32 GetFileCount() const;
        void const *GetPtrConst(void const *base, u32 offset) const
        {
            if (offset == 0)
                return nullptr;

            return ut::AddOffsetToPtr(base, offset);
        }

        void SetStringChunk(void const *stringChunk, u32 stringChunkSize);
        void SetInfoChunk(void const *infoChunk, u32 infoChunkSize);

        bool ReadSeqSoundInfo(u32 soundId,
                              SoundArchive::SeqSoundInfo *info) const;
        bool ReadStrmSoundInfo(u32 soundId,
                               SoundArchive::StrmSoundInfo *info) const;
        bool ReadWaveSoundInfo(u32 soundId,
                               SoundArchive::WaveSoundInfo *info) const;
        bool ReadSoundArchivePlayerInfo(
            SoundArchive::SoundArchivePlayerInfo *info) const;
        bool ReadSoundInfo(u32 soundId, SoundArchive::SoundInfo *info) const;

        bool ReadBankInfo(u32 bankId, SoundArchive::BankInfo *info) const;

        bool ReadPlayerInfo(u32 playerId, SoundArchive::PlayerInfo *info) const;
        bool ReadSound3DParam(u32 soundId, nw4r::snd::SoundArchive::Sound3DParam* info) const;

        bool ReadGroupItemInfo(u32 groupId, u32 index,
                               SoundArchive::GroupItemInfo *info) const;
        bool ReadGroupInfo(u32 groupId, SoundArchive::GroupInfo *info) const;

        bool ReadFileInfo(u32 fileId, SoundArchive::FileInfo *info) const;
        bool ReadFilePos(u32 fileId, u32 index,
                         SoundArchive::FilePos *filePos) const;

        u32 ConvertLabelStringToId(SoundArchiveFile::StringTree const *tree,
                                   char const *str) const;
        const char* GetSoundLabelString(u32 id) const;
        u32 GetSoundUserParam(u32 id) const;

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
        static bool IsValidFileHeader(void const *soundArchiveData);

        SoundArchiveFile::SeqSoundInfo const *impl_GetSeqSoundInfo(
            u32 soundId) const;
        SoundArchiveFile::StrmSoundInfo const *impl_GetStrmSoundInfo(
            u32 soundId) const;
        SoundArchiveFile::WaveSoundInfo const *impl_GetWaveSoundInfo(
            u32 soundId) const;
        SoundArchiveFile::SoundCommonInfo const *impl_GetSoundInfo(
            u32 soundId) const;

        bool impl_GetSoundInfoOffset(
            u32 soundId, SoundArchiveFile::SoundInfoRef *soundInfoRef) const;

        SoundArchiveFile::BankInfo const *impl_GetBankInfo(u32 bankId) const;

        SoundArchiveFile::PlayerInfo const *impl_GetPlayerInfo(
            u32 playerId) const;

        SoundArchiveFile::GroupInfo const *impl_GetGroupInfo(u32 groupId) const;

    public:
        static u16 const SUPPORTED_FILE_VERSION = NW4R_VERSION(1, 4);

    private:
        SoundArchiveFile::Header mHeader;
        SoundArchiveFile::Info const *mInfo;
        void const *mStringBase;
        SoundArchiveFile::StringTable const *mStringTable;
        SoundArchiveFile::StringTree const *mStringTreeSound;
        SoundArchiveFile::StringTree const *mStringTreePlayer;
        SoundArchiveFile::StringTree const *mStringTreeGroup;
        SoundArchiveFile::StringTree const *mStringTreeBank;
    };
}}}

#endif
