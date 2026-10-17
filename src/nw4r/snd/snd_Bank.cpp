#include <nw4r/snd/snd_Bank.h>

#include <nw4r/snd/snd_Channel.h>
#include <nw4r/snd/snd_WaveFile.h>

#include <nw4r/ut/ut_algorithm.h>

namespace nw4r {
namespace snd {
namespace detail {

Bank::Bank(const void* pBankBin)
    : mBankReader(pBankBin), mWaveDataAddress(nullptr) {}

Bank::~Bank() {}

Channel* Bank::NoteOn(const NoteOnInfo& rInfo) const {
    InstInfo instInfo;
    if (!mBankReader.ReadInstInfo(&instInfo, rInfo.prgNo, rInfo.key,
                                  rInfo.velocity)) {
        return nullptr;
    }

    WaveInfo waveInfo;
    const WaveInfo* pWaveInfoAddr;
    if (!mBankReader.ReadWaveInfo(&waveInfo, instInfo.waveDataLocation,
                                  mWaveDataAddress, &pWaveInfoAddr)) {
        return nullptr;
    }

    Channel* pChannel = Channel::AllocChannel(
        ut::Min(waveInfo.numChannels, CHANNEL_MAX), rInfo.voiceOutCount,
        rInfo.priority, rInfo.channelCallback, rInfo.channelCallbackData);

    if (pChannel == nullptr) {
        return nullptr;
    }

    pChannel->SetKey(rInfo.key);
    pChannel->SetOriginalKey(instInfo.originalKey);

    f32 volume = rInfo.velocity / 127.0f;
    volume *= volume;
    volume *= instInfo.volume / 127.0f;
    pChannel->SetInitVolume(volume);

    pChannel->SetTune(instInfo.tune);
    pChannel->SetAttack(instInfo.attack);
    pChannel->SetHold(instInfo.hold);
    pChannel->SetDecay(instInfo.decay);
    pChannel->SetSustain(instInfo.sustain);
    pChannel->SetRelease(instInfo.release);

    f32 pan = (instInfo.pan - 64) / 63.0f;
    pan += rInfo.initPan / 63.0f;
    pChannel->SetInitPan(pan);

    pChannel->SetInitSurroundPan(0.0f);
    pChannel->SetAlternateAssignId(instInfo.alternateAssign);
    pChannel->SetReleaseIgnore(instInfo.noteOffType ==
                               InstInfo::NOTE_OFF_TYPE_IGNORE);

    if (instInfo.waveDataLocation.type ==
        InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_CALLBACK) {
        pChannel->SetWaveDataLocationCallback(
            instInfo.waveDataLocation.callback, pWaveInfoAddr);
    }

    pChannel->Start(waveInfo, rInfo.length, 0);
    return pChannel;
}

}
}
}
