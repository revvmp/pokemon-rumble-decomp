#ifndef GAME_BSOUNDSOURCE_H
#define GAME_BSOUNDSOURCE_H

#include <memory>
#include <nw4r/math/math_types.h>
#include <nw4r/snd/snd_SoundActor.h>
#include <nw4r/snd/snd_SoundHandle.h>

class BSoundSource {
public:
    BSoundSource();
    void SetPosition(nw4r::math::VEC3 position);

protected:
    nw4r::snd::SoundHandle mHandle;
    std::tr1::shared_ptr<nw4r::snd::SoundActor> mActor;
    nw4r::math::VEC3 mSourcePosition;

public:
    virtual ~BSoundSource();
};

#endif
