#ifndef NW4R_SND_SEQ_PLAYER_H
#define NW4R_SND_SEQ_PLAYER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicPlayer.h>
#include <nw4r/snd/snd_DisposeCallbackManager.h>
#include <nw4r/snd/snd_SeqTrack.h>
#include <nw4r/snd/snd_SoundThread.h>

#include <nw4r/ut/ut_Lock.h>

namespace nw4r {
namespace snd {

struct SeqUserprocCallbackParam {
    volatile s16* localVariable;
    volatile s16* globalVariable;
    volatile s16* trackVariable;
    bool cmpFlag;
};

namespace detail {

class Channel;
class NoteOnCallback;
struct NoteOnInfo;
class SeqTrackAllocator;

class SeqPlayer : public BasicPlayer,
                  public DisposeCallback,
                  public SoundThread::PlayerCallback {
public:
    enum OffsetType { OFFSET_TYPE_TICK, OFFSET_TYPE_MILLISEC };

    enum SetupResult {
        SETUP_SUCCESS,
        SETUP_ERR_CANNOT_ALLOCATE_TRACK,
        SETUP_ERR_UNKNOWN
    };

    struct ParserPlayerParam {
        u8 volume;
        u8 priority;
        u8 timebase;
        u16 tempo;
        NoteOnCallback* callback;
    };

    typedef void SeqUserprocCallback(u16 procId,
                                     SeqUserprocCallbackParam* pParam,
                                     void* pCallbackData);

    static const int MAX_SKIP_TICK_PER_FRAME = 768;
    static const int DEFAULT_TEMPO = 120;
    static const int DEFAULT_TIMEBASE = 48;
    static const int VARIABLE_DEFAULT_VALUE = -1;
    static const int TRACK_NUM_PER_PLAYER = 16;
    static const int GLOBAL_VARIABLE_NUM = 16;
    static const int PLAYER_VARIABLE_NUM = 16;
    static const int TRACK_NUM = 16;

public:
    SeqPlayer();
    virtual ~SeqPlayer();

    virtual bool Start();
    virtual void Stop();
    virtual void Pause(bool flag);
    virtual bool IsActive() const {
        return mActiveFlag;
    }
    virtual bool IsStarted() const {
        return mStartedFlag;
    }
    virtual bool IsPause() const {
        return mPauseFlag;
    }

    virtual void InvalidateData(const void* pStart, const void* pEnd);
    virtual void InvalidateWaveData(const void* pStart, const void* pEnd) {}

    virtual void ChannelCallback(Channel* pChannel) {}

    virtual void OnUpdateFrameSoundThread() {
        Update();
    }
    virtual void OnShutdownSoundThread() {
        Stop();
    }

    SetupResult Setup(SeqTrackAllocator* pAllocator, u32 allocTracks,
                      int voiceOutCount, NoteOnCallback* pCallback);
    void Update();
    void Skip(OffsetType offsetType, int offset);
    void Shutdown();

    bool IsReleasePriorityFix() const {
        return mReleasePriorityFixFlag;
    }
    f32 GetPanRange() const {
        return mPanRange;
    }
    int GetVoiceOutCount() const {
        return mVoiceOutCount;
    }
    ParserPlayerParam& GetParserPlayerParam() {
        return mParserParam;
    }

    SeqTrack* GetPlayerTrack(int trackNo);
    volatile s16* GetVariablePtr(int varNo);
    s16 GetLocalVariable(int varNo) const;
    void SetLocalVariable(int varNo, s16 value);
    static void SetGlobalVariable(int varNo, s16 value);

    void SetTempoRatio(f32 tempo);
    void SetReleasePriorityFix(bool fix);
    void SetChannelPriority(int priority);
    void SetSeqUserprocCallback(SeqUserprocCallback* pCallback, void* pArg);
    void SetPlayerTrack(int trackNo, SeqTrack* pTrack);

    f32 CalcTickPerMsec() const {
        return CalcTickPerMinute() / (60 * 1000);
    }
    f32 CalcTickPerMinute() const {
        return mParserParam.timebase * mParserParam.tempo * mTempoRatio;
    }
    u32 GetTickCounter() const {
        return mTickCounter;
    }

    void SetSeqData(const void* pSeqBase, s32 seqOffset);
    void CallSeqUserprocCallback(u16 procId, SeqTrack* pTrack);
    Channel* NoteOn(int bankNo, const NoteOnInfo& rInfo);

    void SetTrackMute(u32 trackFlags, SeqMute mute);
    void SetTrackSilence(u32 trackFlags, bool silence, int fadeFrames);
    void SetTrackVolume(u32 trackFlags, f32 volume);

    static void InitSeqPlayer();

private:
    void InitParam(int voiceOutCount, NoteOnCallback* pCallback);
    void UpdateChannelParam();
    void CloseTrack(int trackNo);
    void FinishPlayer();
    int ParseNextTick(bool doNoteOn);
    void UpdateTick(int msec);
    void SkipTick();

    template <typename T>
    void SetTrackParam(u32 trackFlags, void (SeqTrack::*pSetter)(T),
                       T param) {
        ut::AutoInterruptLock lock;

        for (int i = 0; i < TRACK_NUM && trackFlags != 0;
             trackFlags >>= 1, i++) {
            if (trackFlags & 1) {
                SeqTrack* pTrack = GetPlayerTrack(i);

                if (pTrack != nullptr) {
                    (pTrack->*pSetter)(param);
                }
            }
        }
    }

    template <typename T, typename U>
    void SetTrackParam(u32 trackFlags, void (SeqTrack::*pSetter)(T, U),
                       T param, U param2) {
        ut::AutoInterruptLock lock;

        for (int i = 0; i < TRACK_NUM && trackFlags != 0;
             trackFlags >>= 1, i++) {
            if (trackFlags & 1) {
                SeqTrack* pTrack = GetPlayerTrack(i);

                if (pTrack != nullptr) {
                    (pTrack->*pSetter)(param, param2);
                }
            }
        }
    }

private:
    static s16 mGlobalVariable[GLOBAL_VARIABLE_NUM];

    bool mActiveFlag;
    bool mStartedFlag;
    bool mPauseFlag;
    bool mReleasePriorityFixFlag;
    f32 mPanRange;
    f32 mTempoRatio;
    f32 mTickFraction;
    u32 mSkipTickCounter;
    f32 mSkipTimeCounter;
    s32 mVoiceOutCount;
    ParserPlayerParam mParserParam;
    SeqTrackAllocator* mSeqTrackAllocator;
    SeqUserprocCallback* mSeqUserprocCallback;
    void* mSeqUserprocCallbackArg;
    SeqTrack* mTracks[TRACK_NUM_PER_PLAYER];
    volatile s16 mLocalVariable[PLAYER_VARIABLE_NUM];
    u32 mTickCounter;
};

}
}
}

#endif
