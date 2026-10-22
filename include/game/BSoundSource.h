#ifndef GAME_BSOUNDSOURCE_H
#define GAME_BSOUNDSOURCE_H

#include <memory>
#include <nw4r/math/math_types.h>
#include <nw4r/snd/snd_Sound3DActor.h>
#include <nw4r/snd/snd_SoundHandle.h>

class BSoundSource {
public:
    BSoundSource();
    void SetPosition(nw4r::math::VEC3 position);
    void StartSE(u32 id);
    void StopSE(f32 fadeTime);
    void SetPitch(f32 pitch);
    f32 GetPitch() const;

private:
    void PrepareActor();

protected:
    nw4r::snd::SoundHandle mHandle;
    std::tr1::shared_ptr<nw4r::snd::Sound3DActor> mActor;
    nw4r::math::VEC3 mSourcePosition;

public:
    virtual ~BSoundSource();
};

#endif
