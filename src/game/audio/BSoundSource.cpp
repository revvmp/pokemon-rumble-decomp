#include <game/audio/BSoundSource.h>

#include <game/audio/Sound.h>

namespace {

enum { SE_PLAYER_ID = 0, SE_PLAYABLE_COUNT = 4 };

}

BSoundSource::BSoundSource() {}

BSoundSource::~BSoundSource() {
    if (mActor) {
        mActor->StopAllSound(0);
        mActor.reset();
    }
    mHandle.Stop(0);
}

inline void BSoundSource::PrepareActor() {
    if (!mActor) {
        mActor.reset(new nw4r::snd::Sound3DActor(GetSoundArchivePlayer(), GetSound3DManager()));
        mActor->SetPlayableSoundCount(SE_PLAYER_ID, SE_PLAYABLE_COUNT);
    }
}

void BSoundSource::StartSE(u32 id) {
    PrepareActor();
    mActor->SetPosition(mSourcePosition);
    PlaySound(id, &mHandle, mActor.get());
}

void BSoundSource::StopSE(f32 fadeTime) {
    if (mActor) {
        StopSound(nullptr, mActor.get(), fadeTime);
    }
}

void BSoundSource::SetPosition(nw4r::math::VEC3 position) {
    if (mActor) {
        mActor->SetPosition(position);
    }
    mSourcePosition = position;
}

void BSoundSource::SetPitch(f32 pitch) {
    PrepareActor();
    mActor->SetPitch(pitch);
}

f32 BSoundSource::GetPitch() const {
    if (mActor) {
        return mActor->GetPitch();
    }
    return 1.0f;
}
