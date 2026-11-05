#include <game/script/ScriptAudio.h>
#include <game/script/NPpuVM.h>
#include <game/world/BWorldObj.h>
#include <game/audio/BSoundSource.h>
#include <game/script/CPpuEngine.h>
#include <game/audio/Sound.h>
#include <nw4r/snd/snd_SoundArchivePlayer.h>
#include <nw4r/snd/snd_SoundArchive.h>

void ChangeBGM(u32 id);
void ContinueGameBGM(u32 id);

static inline const char* PopString(NPpu::CEngine* pEngine) {
    NPpu::CValue value;
    pEngine->Pop(value);
    return pEngine->ToString(value);
}

static inline u32 PopSoundID(NPpu::CEngine* pEngine) {
    u32 id = 0;
    if (pEngine->IsIntArg()) {
        id = pEngine->PopInt();
    } else if (pEngine->IsStringArg()) {
        nw4r::snd::SoundArchivePlayer& rPlayer = GetSoundArchivePlayer();
        const char* pLabel = PopString(pEngine);
        id = rPlayer.GetSoundArchive().ConvertLabelStringToSoundId(pLabel);
    } else {
        pEngine->PopReference();
    }
    return id;
}

int PlayBGMTest(NPpu::CEngine* pEngine) {
    ChangeBGM(PopSoundID(pEngine));
    pEngine->PushNull();
    return 0;
}

int StartSETest(NPpu::CEngine* pEngine) {
    PlaySound(pEngine->PopInt());
    pEngine->PushNull();
    return 0;
}

int ContinueBGMTest(NPpu::CEngine* pEngine) {
    ContinueGameBGM(PopSoundID(pEngine));
    pEngine->PushNull();
    return 0;
}

int StopBGM(NPpu::CEngine* pEngine) {
    StopBGM(0.1f);
    pEngine->PushNull();
    return 0;
}

int method_startSE(NPpu::CEngine* pEngine) {
    u32 id = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        BSoundSource* pSource = dynamic_cast<BSoundSource*>(pObject);
        if (pSource != nullptr) {
            pSource->StartSE(id);
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_stopSE(NPpu::CEngine* pEngine) {
    f32 fadeTime = 0.0f;
    if (pEngine->FetchArgCount() > 1) {
        fadeTime = pEngine->PopFloat();
    }
    BSoundSource* pSource = dynamic_cast<BSoundSource*>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pSource != nullptr) {
        pSource->StopSE(fadeTime);
    }
    pEngine->PushNull();
    return 0;
}

int method_getPitch(NPpu::CEngine* pEngine) {
    f32 pitch = 1.0f;
    BSoundSource* pSource = dynamic_cast<BSoundSource*>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pSource != nullptr) {
        pitch = pSource->GetPitch();
    }
    pEngine->PushFloat(pitch);
    return 0;
}

int method_setPitch(NPpu::CEngine* pEngine) {
    f32 pitch = pEngine->PopFloat();
    BSoundSource* pSource = dynamic_cast<BSoundSource*>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pSource != nullptr) {
        pSource->SetPitch(pitch);
    }
    pEngine->PushNull();
    return 0;
}

int GetPokemonVoiceId(u16 species, u16 form);

int GetPokemonVoiceId(NPpu::CEngine* pEngine) {
    u16 form = pEngine->PopInt();
    u16 species = pEngine->PopInt();
    pEngine->PushInt(GetPokemonVoiceId(species, form));
    return 0;
}
