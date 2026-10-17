#include <nw4r/snd/snd_SeqPlayer.h>

#include <nw4r/snd/snd_DisposeCallbackManager.h>
#include <nw4r/snd/snd_NoteOnCallback.h>
#include <nw4r/snd/snd_SeqTrack.h>
#include <nw4r/snd/snd_SeqTrackAllocator.h>
#include <nw4r/snd/snd_SoundThread.h>

#include <nw4r/ut/ut_Lock.h>

namespace nw4r {
namespace snd {
namespace detail {

s16 SeqPlayer::mGlobalVariable[GLOBAL_VARIABLE_NUM];

void SeqPlayer::InitSeqPlayer() {
    for (int i = 0; i < GLOBAL_VARIABLE_NUM; i++) {
        mGlobalVariable[i] = VARIABLE_DEFAULT_VALUE;
    }
}

SeqPlayer::SeqPlayer()
    : mActiveFlag(false),
      mStartedFlag(false),
      mPauseFlag(false),
      mReleasePriorityFixFlag(false),
      mTempoRatio(1.0f),
      mTickFraction(0.0f),
      mSkipTickCounter(0),
      mSkipTimeCounter(0.0f) {
    mPanRange = 1.0f;
    mTickCounter = 0;
    mVoiceOutCount = 0;

    mSeqUserprocCallback = nullptr;
    mSeqUserprocCallbackArg = nullptr;

    mParserParam.tempo = DEFAULT_TEMPO;
    mParserParam.timebase = DEFAULT_TIMEBASE;
    mParserParam.volume = 127;
    mParserParam.priority = 64;
    mParserParam.callback = nullptr;

    for (int i = 0; i < PLAYER_VARIABLE_NUM; i++) {
        mLocalVariable[i] = VARIABLE_DEFAULT_VALUE;
    }

    for (int i = 0; i < TRACK_NUM_PER_PLAYER; i++) {
        mTracks[i] = nullptr;
    }
}

SeqPlayer::~SeqPlayer() {
    Shutdown();
}

void SeqPlayer::InitParam(int voiceOutCount, NoteOnCallback* pCallback) {
    BasicPlayer::InitParam();

    mStartedFlag = false;
    mPauseFlag = false;
    mTempoRatio = 1.0f;
    mSkipTickCounter = 0;
    mSkipTimeCounter = 0.0f;
    mPanRange = 1.0f;
    mTickCounter = 0;
    mVoiceOutCount = voiceOutCount;

    mParserParam.tempo = DEFAULT_TEMPO;
    mParserParam.timebase = DEFAULT_TIMEBASE;
    mParserParam.volume = 127;
    mParserParam.priority = 64;
    mParserParam.callback = pCallback;

    mTickFraction = 0.0f;

    for (int i = 0; i < PLAYER_VARIABLE_NUM; i++) {
        mLocalVariable[i] = VARIABLE_DEFAULT_VALUE;
    }

    for (int i = 0; i < TRACK_NUM_PER_PLAYER; i++) {
        mTracks[i] = nullptr;
    }
}

SeqPlayer::SetupResult SeqPlayer::Setup(SeqTrackAllocator* pAllocator,
                                        u32 allocTracks, int voiceOutCount,
                                        NoteOnCallback* pCallback) {
    SoundThread::AutoLock lock;

    SeqPlayer::Stop();
    InitParam(voiceOutCount, pCallback);

    {
        ut::AutoInterruptLock interruptLock;
        int trackCount = 0;

        for (u32 mask = allocTracks; mask != 0; mask >>= 1) {
            if (mask & 1) {
                trackCount++;
            }
        }

        if (trackCount > pAllocator->GetAllocatableTrackCount()) {
            return SETUP_ERR_CANNOT_ALLOCATE_TRACK;
        }

        u32 mask = allocTracks;

        for (int trackNo = 0; mask != 0; mask >>= 1, trackNo++) {
            if (mask & 1) {
                SeqTrack* pTrack = pAllocator->AllocTrack(this);
                SetPlayerTrack(trackNo, pTrack);
            }
        }
    }

    DisposeCallbackManager::GetInstance().RegisterDisposeCallback(this);

    mSeqTrackAllocator = pAllocator;
    mActiveFlag = true;

    return SETUP_SUCCESS;
}

void SeqPlayer::Shutdown() {
    SoundThread::AutoLock lock;
    FinishPlayer();
}

void SeqPlayer::SetSeqData(const void* pSeqBase, s32 seqOffset) {
    SoundThread::AutoLock lock;
    SeqTrack* pTrack = GetPlayerTrack(0);

    if (pSeqBase != nullptr) {
        pTrack->SetSeqData(pSeqBase, seqOffset);
        pTrack->Open();
    }
}

bool SeqPlayer::Start() {
    SoundThread::AutoLock lock;

    SoundThread::GetInstance().RegisterPlayerCallback(this);
    mStartedFlag = true;

    return true;
}

void SeqPlayer::Stop() {
    SoundThread::AutoLock lock;
    FinishPlayer();
}

void SeqPlayer::Pause(bool flag) {
    SoundThread::AutoLock lock;
    mPauseFlag = flag;

    SeqTrack* pTrack;

    for (int i = 0; i < TRACK_NUM_PER_PLAYER; i++) {
        pTrack = GetPlayerTrack(i);

        if (pTrack != nullptr) {
            pTrack->PauseAllChannel(flag);
        }
    }
}

void SeqPlayer::Skip(OffsetType offsetType, int offset) {
    SoundThread::AutoLock lock;

    if (!mActiveFlag) {
        return;
    }

    switch (offsetType) {
    case OFFSET_TYPE_TICK: {
        mSkipTickCounter += offset;
        break;
    }
    case OFFSET_TYPE_MILLISEC: {
        mSkipTimeCounter += offset;
        break;
    }
    }
}

void SeqPlayer::SetTempoRatio(f32 tempo) {
    mTempoRatio = tempo;
}

void SeqPlayer::SetChannelPriority(int priority) {
    mParserParam.priority = priority;
}

void SeqPlayer::SetReleasePriorityFix(bool fix) {
    mReleasePriorityFixFlag = fix;
}

void SeqPlayer::SetSeqUserprocCallback(SeqUserprocCallback* pCallback,
                                       void* pArg) {
    mSeqUserprocCallback = pCallback;
    mSeqUserprocCallbackArg = pArg;
}

void SeqPlayer::CallSeqUserprocCallback(u16 procId, SeqTrack* pTrack) {
    if (mSeqUserprocCallback == nullptr) {
        return;
    }

    SeqTrack::ParserTrackParam& rTrackParam = pTrack->GetParserTrackParam();

    SeqUserprocCallbackParam param;
    param.localVariable = GetVariablePtr(0);
    param.globalVariable = GetVariablePtr(PLAYER_VARIABLE_NUM);
    param.trackVariable = pTrack->GetVariablePtr(0);
    param.cmpFlag = rTrackParam.cmpFlag;

    (*mSeqUserprocCallback)(procId, &param, mSeqUserprocCallbackArg);

    rTrackParam.cmpFlag = param.cmpFlag;
}

s16 SeqPlayer::GetLocalVariable(int varNo) const {
    return mLocalVariable[varNo];
}

void SeqPlayer::SetLocalVariable(int varNo, s16 value) {
    mLocalVariable[varNo] = value;
}

void SeqPlayer::SetGlobalVariable(int varNo, s16 value) {
    mGlobalVariable[varNo] = value;
}

void SeqPlayer::SetTrackMute(u32 trackFlags, SeqMute mute) {
    SetTrackParam<SeqMute>(trackFlags, &SeqTrack::SetMute, mute);
}

void SeqPlayer::SetTrackSilence(u32 trackFlags, bool silence,
                                int fadeFrames) {
    SetTrackParam<bool, int>(trackFlags, &SeqTrack::SetSilence, silence,
                             fadeFrames);
}

void SeqPlayer::SetTrackVolume(u32 trackFlags, f32 volume) {
    SetTrackParam<f32>(trackFlags, &SeqTrack::SetVolume, volume);
}

void SeqPlayer::InvalidateData(const void* pStart, const void* pEnd) {
    SoundThread::AutoLock lock;

    if (mActiveFlag) {
        for (int i = 0; i < TRACK_NUM_PER_PLAYER; i++) {
            SeqTrack* pTrack = GetPlayerTrack(i);

            if (pTrack == nullptr) {
                continue;
            }

            const u8* pBase = pTrack->GetParserTrackParam().baseAddr;

            if (pStart <= pBase && pBase <= pEnd) {
                SeqPlayer::Stop();
                break;
            }
        }
    }
}

SeqTrack* SeqPlayer::GetPlayerTrack(int trackNo) {
    if (trackNo > TRACK_NUM_PER_PLAYER - 1) {
        return nullptr;
    }

    return mTracks[trackNo];
}

void SeqPlayer::CloseTrack(int trackNo) {
    SoundThread::AutoLock lock;
    SeqTrack* pTrack = GetPlayerTrack(trackNo);

    if (pTrack == nullptr) {
        return;
    }

    pTrack->Close();

    mSeqTrackAllocator->FreeTrack(mTracks[trackNo]);
    mTracks[trackNo] = nullptr;
}

void SeqPlayer::SetPlayerTrack(int trackNo, SeqTrack* pTrack) {
    SoundThread::AutoLock lock;

    if (trackNo > TRACK_NUM_PER_PLAYER - 1) {
        return;
    }

    mTracks[trackNo] = pTrack;
    pTrack->SetPlayerTrackNo(trackNo);
}

void SeqPlayer::FinishPlayer() {
    SoundThread::AutoLock lock;

    if (mStartedFlag) {
        SoundThread::GetInstance().UnregisterPlayerCallback(this);
        mStartedFlag = false;
    }

    if (mActiveFlag) {
        DisposeCallbackManager::GetInstance().UnregisterDisposeCallback(this);
        mActiveFlag = false;
    }

    for (int i = 0; i < TRACK_NUM_PER_PLAYER; i++) {
        CloseTrack(i);
    }
}

void SeqPlayer::UpdateChannelParam() {
    SoundThread::AutoLock lock;
    SeqTrack* pTrack;

    for (int i = 0; i < TRACK_NUM_PER_PLAYER; i++) {
        pTrack = GetPlayerTrack(i);

        if (pTrack != nullptr) {
            pTrack->UpdateChannelParam();
        }
    }
}

int SeqPlayer::ParseNextTick(bool doNoteOn) {
    SoundThread::AutoLock lock;
    bool active = false;

    for (int i = 0; i < TRACK_NUM_PER_PLAYER; i++) {
        SeqTrack* pTrack = GetPlayerTrack(i);

        if (pTrack == nullptr) {
            continue;
        }

        pTrack->UpdateChannelLength();

        if (pTrack->ParseNextTick(doNoteOn) < 0) {
            CloseTrack(i);
        }

        if (pTrack->IsOpened()) {
            active = true;
        }
    }

    if (!active) {
        return TRUE;
    }

    return FALSE;
}

volatile s16* SeqPlayer::GetVariablePtr(int varNo) {
    if (varNo < PLAYER_VARIABLE_NUM) {
        return &mLocalVariable[varNo];
    }

    if (varNo < PLAYER_VARIABLE_NUM + GLOBAL_VARIABLE_NUM) {
        return &mGlobalVariable[varNo - PLAYER_VARIABLE_NUM];
    }

    return nullptr;
}

void SeqPlayer::Update() {
    SoundThread::AutoLock lock;

    if (!mActiveFlag) {
        return;
    }

    if (!mStartedFlag) {
        return;
    }

    if (mSkipTickCounter != 0 || mSkipTimeCounter > 0.0f) {
        SkipTick();
    } else if (!mPauseFlag) {
        UpdateTick(3);
    }

    UpdateChannelParam();
}

void SeqPlayer::UpdateTick(int msec) {
    f32 tickPerMsec = CalcTickPerMsec();

    if (tickPerMsec == 0.0f) {
        return;
    }

    f32 restMsec = static_cast<f32>(msec);
    f32 nextMsec = mTickFraction / tickPerMsec;

    while (nextMsec < restMsec) {
        restMsec -= nextMsec;

        if (ParseNextTick(true)) {
            FinishPlayer();
            return;
        }

        mTickCounter++;

        tickPerMsec = CalcTickPerMsec();

        if (tickPerMsec == 0.0f) {
            return;
        }

        nextMsec = 1.0f / tickPerMsec;
    }

    nextMsec -= restMsec;
    mTickFraction = nextMsec * tickPerMsec;
}

void SeqPlayer::SkipTick() {
    for (int i = 0; i < TRACK_NUM_PER_PLAYER; i++) {
        SeqTrack* pTrack = GetPlayerTrack(i);

        if (pTrack != nullptr) {
            pTrack->ReleaseAllChannel(127);
            pTrack->FreeAllChannel();
        }
    }

    int skipCount = 0;

    while (mSkipTickCounter != 0 ||
           mSkipTimeCounter * CalcTickPerMsec() >= 1.0f) {
        if (skipCount >= MAX_SKIP_TICK_PER_FRAME) {
            return;
        }

        if (mSkipTickCounter != 0) {
            mSkipTickCounter--;
        } else {
            f32 tickPerMsec = CalcTickPerMsec();
            f32 msecPerTick = 1.0f / tickPerMsec;
            mSkipTimeCounter -= msecPerTick;
        }

        if (ParseNextTick(false)) {
            FinishPlayer();
            return;
        }

        skipCount++;
        mTickCounter++;
    }

    mSkipTimeCounter = 0.0f;
}

Channel* SeqPlayer::NoteOn(int bankNo, const NoteOnInfo& rInfo) {
    Channel* pChannel = mParserParam.callback->NoteOn(this, bankNo, rInfo);
    return pChannel;
}

}
}
}
