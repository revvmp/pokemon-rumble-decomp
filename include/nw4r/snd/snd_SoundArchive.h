#ifndef NW4R_SND_SOUND_ARCHIVE_H
#define NW4R_SND_SOUND_ARCHIVE_H

#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Types.h>

namespace nw4r { namespace snd { namespace detail { class SoundArchiveFileReader; }}}

namespace nw4r { namespace ut { class FileStream; }}

namespace nw4r { namespace snd
{

    class SoundArchive
    {

    public:

        enum SoundType
        {
            SOUND_TYPE_INVALID,

            SOUND_TYPE_SEQ,
            SOUND_TYPE_STRM,
            SOUND_TYPE_WAVE,
        };

    public:

        struct BankInfo
        {
            u32 fileId;
        };

        struct FileInfo
        {
            u32 fileSize;
            u32 waveDataFileSize;
            char const *extFilePath;
            u32 filePosCount;
        };

        struct FilePos
        {
            u32 groupId;
            u32 index;
        };

        struct GroupInfo
        {
            u32 itemCount;
            char const *extFilePath;
            u32 offset;
            u32 size;
            u32 waveDataOffset;
            u32 waveDataSize;
        };

        struct GroupItemInfo
        {
            u32 fileId;
            u32 offset;
            u32 size;
            u32 waveDataOffset;
            u32 waveDataSize;
        };

        struct PlayerInfo
        {
            int playableSoundCount;
            u32 heapSize;
        };

        struct SeqSoundInfo
        {
            u32 dataOffset;
            u32 bankId;
            u32 allocTrack;
            int channelPriority;
            bool releasePriorityFixFlag;

        };

        struct StrmSoundInfo
        {
            u32 startPosition;
            u16 allocChannelCount;
            u16 allocTrackFlag;
        };

        struct WaveSoundInfo
        {
            s32 subNo;
            int channelPriority;
            bool releasePriorityFixFlag;

        };

        struct Sound3DParam {
            u32 flags;
            u8 decayCurve;
            u8 decayRatio;
            u8 spatialParameter;
        };

        struct SoundArchivePlayerInfo
        {
            int seqSoundCount;
            int seqTrackCount;
            int strmSoundCount;
            int strmTrackCount;
            int strmChannelCount;
            int waveSoundCount;
            int waveTrackCount;
        };

        struct SoundInfo
        {
            u32 fileId;
            u32 playerId;
            int actorPlayerId;
            int playerPriority;
            int volume;
            int remoteFilter;
            detail::PanMode panMode;
            detail::PanCurve panCurve;
        };

    public:

        SoundArchive();
        virtual ~SoundArchive();

        virtual void const *detail_GetFileAddress(u32 fileId) const = 0;
        virtual void const *detail_GetWaveDataFileAddress(u32 fileId) const = 0;
        virtual int detail_GetRequiredStreamBufferSize() const = 0;
        virtual ut::FileStream *OpenStream(void *buffer, int size, u32 begin,
                                           u32 length) const = 0;
        virtual ut::FileStream *OpenExtStream(void *buffer, int size,
                                              char const *extFilePath,
                                              u32 begin, u32 length) const = 0;

        void Setup(detail::SoundArchiveFileReader *fileReader);
        void Shutdown();

        u32 GetPlayerCount() const;
        u32 GetGroupCount() const;
        bool IsAvailable() const;
        SoundType GetSoundType(u32 soundId) const;

        const char* GetSoundLabelString(u32 id) const;
        u32 ConvertLabelStringToSoundId(char const *label) const;
        u32 ConvertLabelStringToPlayerId(const char* pLabel) const;
        u32 ConvertLabelStringToGroupId(const char* pLabel) const;
        u32 ConvertLabelStringToBankId(const char* pLabel) const;
        u32 GetSoundUserParam(u32 id) const;

        bool ReadSoundInfo(u32 soundId, SoundInfo *info) const;
        bool ReadSeqSoundInfo(u32 soundId, SeqSoundInfo *info) const;
        bool detail_ReadStrmSoundInfo(u32 soundId, StrmSoundInfo *info) const;
        bool detail_ReadWaveSoundInfo(u32 soundId, WaveSoundInfo *info) const;

        bool ReadPlayerInfo(u32 playerId, PlayerInfo *info) const;
        bool ReadSoundArchivePlayerInfo(SoundArchivePlayerInfo *info) const;
        bool detail_ReadSound3DParam(u32 soundId, nw4r::snd::SoundArchive::Sound3DParam*) const;

        bool ReadBankInfo(u32 bankId, BankInfo *info) const;

        bool detail_ReadGroupInfo(u32 groupId, GroupInfo *info) const;
        bool detail_ReadGroupItemInfo(u32 groupId, u32 index,
                                      GroupItemInfo *info) const;

        u32 detail_GetFileCount() const;
        bool detail_ReadFileInfo(u32 fileId, FileInfo *info) const;
        bool detail_ReadFilePos(u32 fileId, u32 index, FilePos *info) const;

        ut::FileStream *detail_OpenFileStream(u32 fileId, void *buffer,
                                              int size) const;
        ut::FileStream* detail_OpenGroupStream(u32 id, void* pBuffer,
                                                int bufferSize) const;
        ut::FileStream* detail_OpenGroupWaveDataStream(u32 id, void* pBuffer,
                                                        int bufferSize) const;

        void SetExternalFileRoot(const char* pExtFileRoot);

    private:
        ut::FileStream *OpenExtStreamImpl(void *buffer, int size,
                                          char const *extFilePath, u32 begin,
                                          u32 length) const;

    public:
        static int const FILE_PATH_MAX = 255;
        static u32 const INVALID_ID = -1;

    private:

        detail::SoundArchiveFileReader *mFileReader;
        char mExtFileRoot[FILE_PATH_MAX + 1];
    };
}}

#endif
