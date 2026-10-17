#include <nw4r/ut/ut_algorithm.h>

#include <nw4r/snd/snd_SeqTrack.h>

#include <nw4r/snd/snd_Channel.h>
#include <nw4r/snd/snd_Lfo.h>
#include <nw4r/snd/snd_MoveValue.h>
#include <nw4r/snd/snd_NoteOnCallback.h>
#include <nw4r/snd/snd_SeqPlayer.h>
#include <nw4r/snd/snd_SoundThread.h>

namespace nw4r {
namespace snd {
namespace detail {

void SeqTrack::SetPlayerTrackNo(int playerTrackNo) {
    mPlayerTrackNo = playerTrackNo;
}

SeqTrack::SeqTrack()
    : mOpenFlag(false), mSeqPlayer(nullptr), mChannelList(nullptr) {
    InitParam();
}

SeqTrack::~SeqTrack() {
    Close();
}

void SeqTrack::InitParam() {
    mExtVolume = 1.0f;
    mExtPitch = 1.0f;
    mExtPan = 0.0f;
    mExtSurroundPan = 0.0f;
    mPanRange = 1.0f;

    mParserTrackParam.baseAddr = nullptr;
    mParserTrackParam.currentAddr = nullptr;
    mParserTrackParam.cmpFlag = true;
    mParserTrackParam.noteWaitFlag = true;
    mParserTrackParam.tieFlag = false;
    mParserTrackParam.monophonicFlag = false;
    mParserTrackParam.callStackDepth = 0;
    mParserTrackParam.wait = 0;
    mParserTrackParam.muteFlag = false;
    mParserTrackParam.silenceFlag = false;
    mParserTrackParam.noteFinishWait = false;
    mParserTrackParam.portaFlag = false;
    mParserTrackParam.damperFlag = false;
    mParserTrackParam.bankNo = 0;
    mParserTrackParam.prgNo = 0;
    mParserTrackParam.lfoParam.Init();
    mParserTrackParam.lfoTarget = 0;
    mParserTrackParam.sweepPitch = 0.0f;
    mParserTrackParam.volume.InitValue(127);
    mParserTrackParam.pan.InitValue(0);
    mParserTrackParam.surroundPan.InitValue(0);
    mParserTrackParam.volume2 = 127;
    mParserTrackParam.velocityRange = 127;
    mParserTrackParam.pitchBend.InitValue(0);
    mParserTrackParam.bendRange = DEFAULT_BENDRANGE;
    mParserTrackParam.initPan = 0;
    mParserTrackParam.transpose = 0;
    mParserTrackParam.priority = DEFAULT_PRIORITY;
    mParserTrackParam.portaKey = DEFAULT_PORTA_KEY;
    mParserTrackParam.portaTime = 0;
    mParserTrackParam.attack = INVALID_ENVELOPE;
    mParserTrackParam.decay = INVALID_ENVELOPE;
    mParserTrackParam.sustain = INVALID_ENVELOPE;
    mParserTrackParam.release = INVALID_ENVELOPE;
    mParserTrackParam.envHold = INVALID_ENVELOPE;
    mParserTrackParam.mainSend = 127;

    for (int i = 0; i < AUX_BUS_NUM; i++) {
        mParserTrackParam.fxSend[i] = 0;
    }

    mParserTrackParam.lpfFreq = 0.0f;
    mParserTrackParam.biquadType = 0;
    mParserTrackParam.biquadValue = 0.0f;

    for (int i = 0; i < TRACK_VARIABLE_NUM; i++) {
        mTrackVariable[i] = SeqPlayer::VARIABLE_DEFAULT_VALUE;
    }
}

void SeqTrack::SetSeqData(const void* pSeqBase, s32 seqOffset) {
    mParserTrackParam.baseAddr = static_cast<const u8*>(pSeqBase);
    mParserTrackParam.currentAddr = mParserTrackParam.baseAddr + seqOffset;
}

void SeqTrack::Open() {
    mParserTrackParam.noteFinishWait = false;
    mParserTrackParam.callStackDepth = 0;
    mParserTrackParam.wait = 0;

    mOpenFlag = true;
}

void SeqTrack::Close() {
    SoundThread::AutoLock lock;

    ReleaseAllChannel(MUTE_RELEASE_VALUE);
    FreeAllChannel();

    mOpenFlag = false;
}

void SeqTrack::UpdateChannelLength() {
    SoundThread::AutoLock lock;

    if (!mOpenFlag) {
        return;
    }

    for (Channel* pChannel = mChannelList; pChannel != nullptr;
         pChannel = pChannel->GetNextTrackChannel()) {
        if (pChannel->GetLength() > 0) {
            pChannel->SetLength(pChannel->GetLength() - 1);
        }

        UpdateChannelRelease(pChannel);

        if (!pChannel->IsAutoUpdateSweep()) {
            pChannel->UpdateSweep(1);
        }
    }
}

void SeqTrack::UpdateChannelRelease(Channel* pChannel) {
    if (pChannel->GetLength() == 0 && !pChannel->IsRelease() &&
        !mParserTrackParam.damperFlag) {
        pChannel->NoteOff();
    }
}

int SeqTrack::ParseNextTick(bool doNoteOn) {
    SoundThread::AutoLock lock;

    if (!mOpenFlag) {
        return 0;
    }

    mParserTrackParam.volume.Update();
    mParserTrackParam.pan.Update();
    mParserTrackParam.surroundPan.Update();
    mParserTrackParam.pitchBend.Update();

    if (mParserTrackParam.noteFinishWait) {
        if (mChannelList != nullptr) {
            return 1;
        }

        mParserTrackParam.noteFinishWait = false;
    }

    if (mParserTrackParam.wait > 0 && --mParserTrackParam.wait > 0) {
        return 1;
    }

    if (mParserTrackParam.currentAddr != nullptr) {
        while (mParserTrackParam.wait == 0 &&
               !mParserTrackParam.noteFinishWait) {
            ParseResult result = Parse(doNoteOn);

            if (result == PARSE_RESULT_FINISH) {
                return -1;
            }
        }
    }

    return 1;
}

void SeqTrack::StopAllChannel() {
    SoundThread::AutoLock lock;

    for (Channel* pChannel = mChannelList; pChannel != nullptr;
         pChannel = pChannel->GetNextTrackChannel()) {
        Channel::FreeChannel(pChannel);
        pChannel->Stop();
    }

    mChannelList = nullptr;
}

void SeqTrack::ReleaseAllChannel(int release) {
    SoundThread::AutoLock lock;

    UpdateChannelParam();

    for (Channel* pChannel = mChannelList; pChannel != nullptr;
         pChannel = pChannel->GetNextTrackChannel()) {
        if (pChannel->IsActive()) {
            if (release >= 0) {
                pChannel->SetRelease(static_cast<u8>(release));
            }

            pChannel->Release();
        }
    }
}

void SeqTrack::PauseAllChannel(bool flag) {
    SoundThread::AutoLock lock;

    for (Channel* pChannel = mChannelList; pChannel != nullptr;
         pChannel = pChannel->GetNextTrackChannel()) {
        if (pChannel->IsActive() && flag != pChannel->IsPause()) {
            pChannel->Pause(flag);
        }
    }
}

void SeqTrack::AddChannel(Channel* pChannel) {
    SoundThread::AutoLock lock;

    pChannel->SetNextTrackChannel(mChannelList);
    mChannelList = pChannel;
}

void SeqTrack::UpdateChannelParam() {
    SoundThread::AutoLock lock;

    if (!mOpenFlag) {
        return;
    }

    if (mChannelList == nullptr) {
        return;
    }

    f32 volume = 1.0f;
    f32 parserVolume = mParserTrackParam.volume.GetValue() / 127.0f;
    f32 parserVolume2 = mParserTrackParam.volume2 / 127.0f;
    f32 parserMainVolume =
        mSeqPlayer->GetParserPlayerParam().volume / 127.0f;

    volume *= parserVolume * parserVolume;
    volume *= parserVolume2 * parserVolume2;
    volume *= parserMainVolume * parserMainVolume;
    volume *= mExtVolume;
    volume *= mSeqPlayer->GetVolume();

    f32 pitch = mParserTrackParam.pitchBend.GetValue() / 128.0f *
                mParserTrackParam.bendRange;

    f32 pitchRatio = 1.0f;
    pitchRatio *= mSeqPlayer->GetPitch();
    pitchRatio *= mExtPitch;

    f32 pan = 0.0f;
    pan += ut::Clamp(mParserTrackParam.pan.GetValue() / 63.0f, -1.0f, 1.0f);
    pan *= mPanRange;
    pan *= mSeqPlayer->GetPanRange();
    pan += mExtPan;
    pan += mSeqPlayer->GetPan();

    f32 surroundPan = 0.0f;
    surroundPan += ut::Clamp(mParserTrackParam.surroundPan.GetValue() / 63.0f,
                             0.0f, 2.0f);
    surroundPan += mExtSurroundPan;
    surroundPan += mSeqPlayer->GetSurroundPan();

    f32 lpfFreq = 0.0f;
    lpfFreq += mParserTrackParam.lpfFreq;
    lpfFreq += mSeqPlayer->GetLpfFreq();

    int biquadType = mParserTrackParam.biquadType;
    f32 biquadValue = mParserTrackParam.biquadValue;

    if (mSeqPlayer->GetBiquadType() != 0) {
        biquadType = mSeqPlayer->GetBiquadType();
        biquadValue = mSeqPlayer->GetBiquadValue();
    }

    int remoteFilter = 0;
    remoteFilter += mSeqPlayer->GetRemoteFilter();

    f32 mainSend = 0.0f;
    mainSend += mParserTrackParam.mainSend / 127.0f - 1.0f;
    mainSend += mSeqPlayer->GetMainSend();

    f32 fxSend[AUX_BUS_NUM];

    for (int i = 0; i < AUX_BUS_NUM; i++) {
        AuxBus bus = static_cast<AuxBus>(i);
        fxSend[i] = 0.0f;
        fxSend[i] += mParserTrackParam.fxSend[i] / 127.0f;
        fxSend[i] += mSeqPlayer->GetFxSend(bus);
    }

    for (Channel* pChannel = mChannelList; pChannel != nullptr;
         pChannel = pChannel->GetNextTrackChannel()) {
        pChannel->SetUserVolume(volume);
        pChannel->SetUserPitch(pitch);
        pChannel->SetUserPitchRatio(pitchRatio);
        pChannel->SetUserPan(pan);
        pChannel->SetUserSurroundPan(surroundPan);
        pChannel->SetUserLpfFreq(lpfFreq);
        pChannel->SetBiquadFilter(biquadType, biquadValue);
        pChannel->SetRemoteFilter(remoteFilter);
        pChannel->SetOutputLine(mSeqPlayer->GetOutputLine());
        pChannel->SetMainOutVolume(mSeqPlayer->GetMainOutVolume());
        pChannel->SetMainSend(mainSend);

        for (int i = 0; i < AUX_BUS_NUM; i++) {
            AuxBus bus = static_cast<AuxBus>(i);
            pChannel->SetFxSend(bus, fxSend[i]);
        }

        for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
            pChannel->SetRemoteOutVolume(i, mSeqPlayer->GetRemoteOutVolume(i));
        }

        for (int i = 0; i < mSeqPlayer->GetVoiceOutCount(); i++) {
            pChannel->SetVoiceOutParam(i, mSeqPlayer->GetVoiceOutParam(i));
        }

        pChannel->SetLfoParam(mParserTrackParam.lfoParam);
        pChannel->SetLfoTarget(
            static_cast<Channel::LfoTarget>(mParserTrackParam.lfoTarget));
    }
}

void SeqTrack::FreeAllChannel() {
    SoundThread::AutoLock lock;

    for (Channel* pChannel = mChannelList; pChannel != nullptr;
         pChannel = pChannel->GetNextTrackChannel()) {
        Channel::FreeChannel(pChannel);
    }

    mChannelList = nullptr;
}

void SeqTrack::ChannelCallbackFunc(Channel* pDropChannel,
                                   Channel::ChannelCallbackStatus status,
                                   u32 userData) {
    SoundThread::AutoLock lock;
    SeqTrack* pTrack = reinterpret_cast<SeqTrack*>(userData);

    switch (status) {
    case Channel::CALLBACK_STATUS_STOPPED:
    case Channel::CALLBACK_STATUS_FINISH: {
        Channel::FreeChannel(pDropChannel);
        break;
    }
    }

    if (pTrack->mSeqPlayer != nullptr) {
        pTrack->mSeqPlayer->ChannelCallback(pDropChannel);
    }

    if (pTrack->mChannelList == pDropChannel) {
        pTrack->mChannelList = pDropChannel->GetNextTrackChannel();
        return;
    }

    for (Channel* pChannel = pTrack->mChannelList;
         pChannel->GetNextTrackChannel() != nullptr;
         pChannel = pChannel->GetNextTrackChannel()) {
        if (pChannel->GetNextTrackChannel() == pDropChannel) {
            pChannel->SetNextTrackChannel(pDropChannel->GetNextTrackChannel());
            return;
        }
    }
}

void SeqTrack::SetMute(SeqMute mute) {
    SoundThread::AutoLock lock;

    switch (mute) {
    case MUTE_OFF: {
        mParserTrackParam.muteFlag = false;
        break;
    }
    case MUTE_STOP: {
        StopAllChannel();
        mParserTrackParam.muteFlag = true;
        break;
    }
    case MUTE_RELEASE: {
        ReleaseAllChannel(-1);
        FreeAllChannel();
        mParserTrackParam.muteFlag = true;
        break;
    }
    case MUTE_NO_STOP: {
        mParserTrackParam.muteFlag = true;
        break;
    }
    }
}

void SeqTrack::SetSilence(bool silenceFlag, int fadeTimes) {
    SoundThread::AutoLock lock;
    mParserTrackParam.silenceFlag = silenceFlag;

    for (Channel* pChannel = mChannelList; pChannel != nullptr;
         pChannel = pChannel->GetNextTrackChannel()) {
        pChannel->SetSilence(silenceFlag, (fadeTimes + 2) / 3);
    }
}

void SeqTrack::SetVolume(f32 volume) {
    mExtVolume = volume;
}

void SeqTrack::SetPitch(f32 pitch) {
    mExtPitch = pitch;
}

void SeqTrack::SetPan(f32 pan) {
    mExtPan = pan;
}

void SeqTrack::SetSurroundPan(f32 surroundPan) {
    mExtSurroundPan = surroundPan;
}

void SeqTrack::SetPanRange(f32 panRange) {
    mPanRange = panRange;
}

void SeqTrack::SetLpfFreq(f32 lpfFreq) {
    mParserTrackParam.lpfFreq = lpfFreq;
}

void SeqTrack::SetBiquadFilter(int type, f32 value) {
    mParserTrackParam.biquadType = type;
    mParserTrackParam.biquadValue = value;
}

void SeqTrack::SetModDepth(f32 modDepth) {
    mParserTrackParam.lfoParam.depth = modDepth;
}

void SeqTrack::SetModSpeed(f32 modSpeed) {
    mParserTrackParam.lfoParam.speed = modSpeed;
}

void SeqTrack::SetTrackVariable(int varNo, s16 value) {
    mTrackVariable[varNo] = value;
}

volatile s16* SeqTrack::GetVariablePtr(int varNo) {
    if (varNo < TRACK_VARIABLE_NUM) {
        return &mTrackVariable[varNo];
    }

    return nullptr;
}

Channel* SeqTrack::NoteOn(int key, int velocity, s32 length, bool tieFlag) {
    SoundThread::AutoLock lock;

    const SeqPlayer::ParserPlayerParam& rPlayerParam =
        mSeqPlayer->GetParserPlayerParam();

    Channel* pChannel = nullptr;
    velocity = velocity * mParserTrackParam.velocityRange / 127;

    if (tieFlag) {
        pChannel = GetLastChannel();

        if (pChannel != nullptr) {
            pChannel->SetKey(static_cast<u8>(key));

            f32 initVolume = velocity / 127.0f;
            pChannel->SetInitVolume(initVolume * initVolume);
        }
    }

    if (GetParserTrackParam().monophonicFlag) {
        pChannel = GetLastChannel();

        if (pChannel != nullptr) {
            if (pChannel->IsRelease()) {
                pChannel->Stop();
                pChannel = nullptr;
            } else {
                pChannel->SetKey(static_cast<u8>(key));

                f32 initVolume = velocity / 127.0f;
                pChannel->SetInitVolume(initVolume * initVolume);
                pChannel->SetLength(length);
            }
        }
    }

    if (pChannel == nullptr) {
        NoteOnInfo info = {
            mParserTrackParam.prgNo,
            key,
            velocity,
            tieFlag ? -1 : length,
            mParserTrackParam.initPan,
            rPlayerParam.priority + GetParserTrackParam().priority,
            mSeqPlayer->GetVoiceOutCount(),
            &ChannelCallbackFunc,
            reinterpret_cast<u32>(this)};

        pChannel = mSeqPlayer->NoteOn(mParserTrackParam.bankNo, info);

        if (pChannel == nullptr) {
            return nullptr;
        }

        if (pChannel->GetAlternateAssignId() > 0) {
            for (Channel* pIt = mChannelList; pIt != nullptr;
                 pIt = pIt->GetNextTrackChannel()) {
                if (pIt->GetAlternateAssignId() ==
                    pChannel->GetAlternateAssignId()) {
                    pIt->Release();
                }
            }
        }

        AddChannel(pChannel);
    }

    if (mParserTrackParam.attack <= MAX_ENVELOPE_VALUE) {
        pChannel->SetAttack(mParserTrackParam.attack);
    }

    if (mParserTrackParam.decay <= MAX_ENVELOPE_VALUE) {
        pChannel->SetDecay(mParserTrackParam.decay);
    }

    if (mParserTrackParam.sustain <= MAX_ENVELOPE_VALUE) {
        pChannel->SetSustain(mParserTrackParam.sustain);
    }

    if (mParserTrackParam.release <= MAX_ENVELOPE_VALUE) {
        pChannel->SetRelease(mParserTrackParam.release);
    }

    if (mParserTrackParam.envHold <= MAX_ENVELOPE_VALUE) {
        pChannel->SetHold(mParserTrackParam.envHold);
    }

    f32 sweepPitch = mParserTrackParam.sweepPitch;

    if (mParserTrackParam.portaFlag) {
        sweepPitch += mParserTrackParam.portaKey - key;
    }

    if (mParserTrackParam.portaTime == 0) {
        pChannel->SetSweepParam(sweepPitch, length, false);
    } else {
        int sweepTime = mParserTrackParam.portaTime;
        sweepTime *= sweepTime;
        sweepTime *= sweepPitch >= 0.0f ? sweepPitch : -sweepPitch;
        sweepTime >>= 5;
        sweepTime *= 5;

        pChannel->SetSweepParam(sweepPitch, sweepTime, true);
    }

    mParserTrackParam.portaKey = key;

    pChannel->SetSilence(mParserTrackParam.silenceFlag != false, 0);
    pChannel->SetReleasePriorityFix(mSeqPlayer->IsReleasePriorityFix());
    pChannel->SetPanMode(mSeqPlayer->GetPanMode());
    pChannel->SetPanCurve(mSeqPlayer->GetPanCurve());

    return pChannel;
}

}
}
}
