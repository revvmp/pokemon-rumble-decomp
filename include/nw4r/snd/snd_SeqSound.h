#ifndef NW4R_SND_SEQ_SOUND_H
#define NW4R_SND_SEQ_SOUND_H

#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_debug.h>
#include <nw4r/snd/snd_SeqPlayer.h>
#include <nw4r/snd/snd_Task.h>

#include <nw4r/ut/ut_LinkList.h>

namespace nw4r { namespace snd { namespace detail { class NoteOnCallback; }}}
namespace nw4r { namespace snd { namespace detail { class SeqTrackAllocator; }}}
namespace nw4r { namespace snd { namespace detail { template <class> class SoundInstanceManager; }}}
namespace nw4r { namespace snd { class SeqSoundHandle; }}

namespace nw4r { namespace ut { namespace detail { class RuntimeTypeInfo; }}}
namespace nw4r { namespace ut { class FileStream; }}

namespace nw4r { namespace snd { namespace detail
{

    class SeqSound : public BasicSound
    {

    public:
        typedef ut::LinkList<SeqSound, 0xf0> PriorityLinkList;

    public:
        class SeqLoadTask : public Task
        {

        public:
            typedef void Callback(bool result, void const *seqBase,
                                  void *userData);

        public:
            SeqLoadTask();

            virtual void Execute();
            virtual void Cancel();
            virtual void OnCancel();

        public:
            ut::FileStream *mFileStream;
            void *mBuffer;
            int mBufferSize;
            Callback *mCallback;
            void *mCallbackData;
        };

    public:
        SeqSound(SoundInstanceManager<SeqSound> *manager, int priority,
                 int ambientPriority);

        virtual ut::detail::RuntimeTypeInfo const *GetRuntimeTypeInfo() const
        {
            return &typeInfo;
        }
        virtual void Shutdown();
        virtual bool IsPrepared() const { return mPreparedFlag; }
        virtual bool IsAttachedTempSpecialHandle();
        virtual void DetachTempSpecialHandle();
        virtual void InitParam();
        virtual BasicPlayer &GetBasicPlayer() { return mSeqPlayer; }
        virtual BasicPlayer const &GetBasicPlayer() const { return mSeqPlayer; }
        virtual void OnUpdatePlayerPriority();

        SeqPlayer::SetupResult Setup(SeqTrackAllocator *trackAllocator,
                                     u32 allocTracks,
                                     NoteOnCallback *callback);
        void Prepare(void const *seqBase, s32 seqOffset,
                     SeqPlayer::OffsetType startOffsetType, int startOffset);
        void Prepare(ut::FileStream *fileStream, s32 seqOffset,
                     SeqPlayer::OffsetType startOffsetType, int startOffset);

        void *GetFileStreamBuffer() { return mFileStreamBuffer; }
        s32 GetFileStreamBufferSize() { return sizeof mFileStreamBuffer; }

        void SetReleasePriorityFix(bool fix);
        void SetTempoRatio(f32 tempo);
        void SetChannelPriority(int priority);
        void SetSeqUserprocCallback(SeqPlayer::SeqUserprocCallback *callback,
                                    void *arg);

        void Skip(SeqPlayer::OffsetType offsetType, int offset);

        bool LoadData(SeqLoadTask::Callback *callback,
                      void *callbackArg);

        static void NotifyLoadAsyncEndSeqData(bool result, void const *seqBase,
                                              void *userData);

        void SetTrackMute(u32 trackFlags, SeqMute mute);
        void SetTrackSilence(u32 trackFlags, bool silence, int fadeFrames);
        void SetTrackVolume(u32 trackFlags, f32 volume);
        bool ReadVariable(int varNo, s16 *value) const;
        bool WriteVariable(int varNo, s16 value);
        static bool WriteGlobalVariable(int varNo, s16 value);
        bool WriteTrackVariable(int trackNo, int varNo, s16 value);
        u32 GetTick() const;

    public:
        static int const FILE_STREAM_BUFFER_SIZE = 128;

        static ut::detail::RuntimeTypeInfo const typeInfo;

    private:
        SeqPlayer mSeqPlayer;
        SeqSoundHandle *mTempSpecialHandle;
        SoundInstanceManager<SeqSound> *mManager;
        s32 mSeqOffset;
        SeqPlayer::OffsetType mStartOffsetType;
        int mStartOffset;
        bool volatile mLoadingFlag;
        bool volatile mPreparedFlag;

        ut::FileStream *mFileStream;
        int mFileStreamBuffer[FILE_STREAM_BUFFER_SIZE];
        SeqLoadTask mSeqLoadTask;

    private:
        friend class snd::SeqSoundHandle;
    };
}}}

#endif
