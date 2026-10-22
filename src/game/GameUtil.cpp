#include <game/GameUtil.h>

#include <nw4r/snd/snd_SoundArchivePlayer.h>
#include <nw4r/snd/snd_AxManager.h>
#include <nw4r/snd/snd_FxChorus.h>
#include <nw4r/snd/snd_FxDelay.h>
#include <nw4r/snd/snd_FxReverbStd.h>
#include <nw4r/snd/snd_SoundArchive.h>
#include <nw4r/snd/snd_SoundPlayer.h>
#include <game/Sound.h>
#include <game/StageDesc.h>

namespace {

enum { TYPE_COUNT = 18 };

enum FxKind {
    FX_NONE,
    FX_REVERB = 2,
    FX_CHORUS = 3,
    FX_DELAY = 4
};

enum { FX_SLOT_COUNT = 3, FX_PRESET_COUNT = 10 };

enum { PLAYER_NO_PAUSE = 2 };

struct BgmFxPreset {
    nw4r::snd::detail::FxReverbStdParam mReverb;
    nw4r::snd::detail::FxDelayParam mDelay;
    nw4r::snd::detail::FxChorusParam mChorus;
    int mEffects[nw4r::snd::AUX_BUS_NUM][FX_SLOT_COUNT];
};

}

static const s8 sTypeEfficacy[TYPE_COUNT][TYPE_COUNT] = {
    {3, 3, 3, 3, 3, 2, 3, 0, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {4, 3, 2, 2, 3, 4, 2, 0, 4, 3, 3, 3, 3, 3, 2, 4, 3, 4},
    {3, 4, 3, 3, 3, 2, 4, 3, 2, 3, 3, 3, 4, 2, 3, 3, 3, 3},
    {3, 3, 3, 2, 2, 2, 3, 2, 0, 3, 3, 3, 4, 3, 3, 3, 3, 3},
    {3, 3, 0, 4, 3, 4, 2, 3, 4, 3, 4, 3, 2, 4, 3, 3, 3, 3},
    {3, 2, 4, 3, 2, 3, 4, 3, 2, 3, 4, 3, 3, 3, 3, 4, 3, 3},
    {3, 2, 2, 2, 3, 3, 3, 2, 2, 3, 2, 3, 4, 3, 4, 3, 3, 4},
    {0, 3, 3, 3, 3, 3, 3, 4, 2, 3, 3, 3, 3, 3, 4, 3, 3, 2},
    {3, 3, 3, 3, 3, 4, 3, 3, 2, 3, 2, 2, 3, 2, 3, 4, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 2, 4, 3, 4, 3, 2, 2, 4, 3, 3, 4, 2, 3},
    {3, 3, 3, 3, 4, 4, 3, 3, 3, 3, 4, 2, 2, 3, 3, 3, 2, 3},
    {3, 3, 2, 2, 4, 4, 2, 3, 2, 3, 2, 4, 2, 3, 3, 3, 2, 3},
    {3, 3, 4, 3, 0, 3, 3, 3, 3, 3, 3, 4, 2, 2, 3, 3, 2, 3},
    {3, 4, 3, 4, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 2, 3, 3, 0},
    {3, 3, 4, 3, 4, 3, 3, 3, 2, 3, 2, 2, 4, 3, 3, 2, 4, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 4, 3},
    {3, 2, 3, 3, 3, 3, 3, 4, 2, 3, 3, 3, 3, 3, 4, 3, 3, 2},
};

static BgmFxPreset sBgmFxPresets[FX_PRESET_COUNT] = {
    {},
    {{}, {}, {}, {{FX_REVERB}}},
    {{}, {}, {}, {{FX_REVERB}, {FX_DELAY}}},
    {{}, {}, {}, {{FX_REVERB}, {FX_DELAY}}},
    {{}, {}, {}, {{FX_REVERB}, {FX_DELAY}, {FX_CHORUS}}},
    {{}, {}, {}, {{FX_REVERB}, {FX_DELAY}, {FX_CHORUS}}},
    {{}, {}, {}, {{FX_REVERB}, {FX_DELAY}, {FX_CHORUS}}},
    {{}, {}, {}, {{FX_REVERB}, {FX_DELAY}}},
    {{}, {}, {}, {{FX_REVERB}, {FX_DELAY}}},
    {{}, {}, {}, {{FX_REVERB}, {FX_DELAY}, {FX_CHORUS}}},
};
static bool sBgmFxPresetsInitialized;
static u32 sCurrentBgmFxPreset;
static int sAudioPauseCount;

bool IsStage7EvenMap(MapIndex index) {
    return index.mStage == 7 && index.mMap % 2 == 0;
}

bool IsBattleRoyalFirstPart(MapIndex index) {
    return index.mMap < 3;
}

bool IsStage7OddMap(MapIndex index) {
    return index.mStage == 7 && index.mMap % 2 == 1;
}

void StageDesc::Init(MapIndex index) {
    mIndex = index;
    mParamTable.Reset();
    std::vector<std::tr1::shared_ptr<MapDesc> > empty;
    mMapDescs.swap(empty);
}

int GetTypeEfficacy(int attackType, int defendType) {
    return sTypeEfficacy[attackType][defendType];
}

inline void SetReverbParam(nw4r::snd::detail::FxReverbStdParam& rParam, f32 fusedTime, f32 damping, u32 fusedMode) {
    rParam.preDelayTime = 0.02f;
    rParam.fusedTime = fusedTime;
    rParam.coloration = 0.6f;
    rParam.damping = damping;
    rParam.outGain = 1.0f;
    rParam.earlyMode = 5;
    rParam.preDelayTimeMax = 0.15f;
    rParam.fusedMode = fusedMode;
    rParam.earlyGain = 0.0f;
    rParam.fusedGain = 1.0f;
}

inline void SetDelayParam(nw4r::snd::detail::FxDelayParam& rParam, f32 delay, f32 feedback) {
    rParam.delay = delay;
    rParam.feedback = feedback;
    rParam.outGain = 1.0f;
    rParam.maxDelay = 500.0f;
    rParam.lpf = 1.0f;
}

inline void SetChorusParam(nw4r::snd::detail::FxChorusParam& rParam) {
    rParam.delayTime = 0.8f;
    rParam.depth = 0.9f;
    rParam.rate = 0.5f;
    rParam.feedback = 0.9f;
    rParam.outGain = 1.0f;
}

static void InitBgmFxPresets() {
    if (sBgmFxPresetsInitialized) {
        return;
    }
    sBgmFxPresetsInitialized = true;

    SetReverbParam(sBgmFxPresets[1].mReverb, 3.0f, 0.4f, 0);

    SetReverbParam(sBgmFxPresets[2].mReverb, 3.0f, 0.4f, 0);
    SetDelayParam(sBgmFxPresets[2].mDelay, 176.47f, 0.4f);

    SetReverbParam(sBgmFxPresets[3].mReverb, 2.5f, 0.2f, 4);
    SetDelayParam(sBgmFxPresets[3].mDelay, 166.66f, 0.4f);

    SetReverbParam(sBgmFxPresets[4].mReverb, 2.5f, 0.2f, 4);
    SetDelayParam(sBgmFxPresets[4].mDelay, 176.47f, 0.4f);
    SetChorusParam(sBgmFxPresets[4].mChorus);

    SetReverbParam(sBgmFxPresets[5].mReverb, 2.5f, 0.2f, 4);
    SetDelayParam(sBgmFxPresets[5].mDelay, 222.22f, 0.8f);
    SetChorusParam(sBgmFxPresets[5].mChorus);

    SetReverbParam(sBgmFxPresets[6].mReverb, 2.5f, 0.2f, 4);
    SetDelayParam(sBgmFxPresets[6].mDelay, 300.0f, 0.8f);
    SetChorusParam(sBgmFxPresets[6].mChorus);

    SetReverbParam(sBgmFxPresets[7].mReverb, 2.5f, 0.2f, 4);
    SetDelayParam(sBgmFxPresets[7].mDelay, 437.5f, 0.4f);

    SetReverbParam(sBgmFxPresets[8].mReverb, 3.0f, 0.4f, 0);
    SetDelayParam(sBgmFxPresets[8].mDelay, 437.5f, 0.4f);

    SetReverbParam(sBgmFxPresets[9].mReverb, 2.5f, 0.2f, 4);
    SetDelayParam(sBgmFxPresets[9].mDelay, 273.0f, 0.6f);
    SetChorusParam(sBgmFxPresets[9].mChorus);
}

static void ApplyBgmFxPreset(u32 id) {
    if (id != 0) {
        BgmFxPreset& rPreset = sBgmFxPresets[id];
        bool useReverb = false;
        bool useChorus = false;
        bool useDelay = false;
        for (int bus = 0; bus < nw4r::snd::AUX_BUS_NUM; bus++) {
            for (int slot = 0; slot < FX_SLOT_COUNT; slot++) {
                switch (rPreset.mEffects[bus][slot]) {
                case FX_REVERB:
                    useReverb = true;
                    break;
                case FX_CHORUS:
                    useChorus = true;
                    break;
                case FX_DELAY:
                    useDelay = true;
                    break;
                }
            }
        }

        if (useReverb) {
            GetFxReverb().SetParam(rPreset.mReverb);
        }
        if (useChorus) {
            GetFxChorus().SetParam(rPreset.mChorus);
        }
        if (useDelay) {
            GetFxDelay().SetParam(rPreset.mDelay);
        }

        for (int bus = 0; bus <= nw4r::snd::AUX_C; bus++) {
            for (int slot = 0; slot < FX_SLOT_COUNT; slot++) {
                switch (rPreset.mEffects[bus][slot]) {
                case FX_REVERB:
                    nw4r::snd::detail::AxManager::GetInstance().AppendEffect(static_cast<nw4r::snd::AuxBus>(bus), &GetFxReverb());
                    break;
                case FX_CHORUS:
                    nw4r::snd::detail::AxManager::GetInstance().AppendEffect(static_cast<nw4r::snd::AuxBus>(bus), &GetFxChorus());
                    break;
                case FX_DELAY:
                    nw4r::snd::detail::AxManager::GetInstance().AppendEffect(static_cast<nw4r::snd::AuxBus>(bus), &GetFxDelay());
                    break;
                }
            }
        }
    }
    sCurrentBgmFxPreset = id;
}

static void SwitchBgmFxPreset(u32 bgm) {
    u32 preset = GetSoundArchivePlayer().GetSoundArchive().GetSoundUserParam(bgm);
    if (preset != sCurrentBgmFxPreset) {
        ClearGameAudioEffects();
        ApplyBgmFxPreset(preset);
    }
}

void ChangeBGM(u32 id) {
    InitBgmFxPresets();
    if (id != GetBGMID()) {
        SwitchBgmFxPreset(id);
        PlayBGM(id, 0.0f, false);
    }
}

void ContinueGameBGM(u32 id) {
    if (id != GetBGMID()) {
        SwitchBgmFxPreset(id);
        ContinueBGM(id, -1.0f);
    }
}

void PauseGameAudio() {
    if (sAudioPauseCount++ <= 0) {
        u32 count = GetSoundArchivePlayer().GetSoundPlayerCount();
        for (int i = 0; i < count; i++) {
            if (i != PLAYER_NO_PAUSE) {
                GetSoundArchivePlayer().GetSoundPlayer(i).PauseAllSound(true, 0);
            }
        }
        SetBGMVolume(0.3f, 0.1f);
    }
}

void ResumeGameAudio() {
    if (--sAudioPauseCount <= 0) {
        u32 count = GetSoundArchivePlayer().GetSoundPlayerCount();
        for (int i = 0; i < count; i++) {
            if (i != PLAYER_NO_PAUSE) {
                GetSoundArchivePlayer().GetSoundPlayer(i).PauseAllSound(false, 0);
            }
        }
        SetBGMVolume(1.0f, 0.1f);
    }
}

void ClearGameAudioEffects() {
    nw4r::snd::detail::AxManager::GetInstance().ClearEffect(nw4r::snd::AUX_A, 0);
    nw4r::snd::detail::AxManager::GetInstance().ClearEffect(nw4r::snd::AUX_B, 0);
    nw4r::snd::detail::AxManager::GetInstance().ClearEffect(nw4r::snd::AUX_C, 0);
}

void RestoreGameAudioEffects() {
    ApplyBgmFxPreset(sCurrentBgmFxPreset);
}
