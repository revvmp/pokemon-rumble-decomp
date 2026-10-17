#include <nw4r/snd/snd_SeqSoundHandle.h>

namespace nw4r {
namespace snd {

void SeqSoundHandle::DetachSound() {
    if (IsAttachedSound()) {
        if (mSound->mTempSpecialHandle == this)
            mSound->mTempSpecialHandle = nullptr;
    }

    if (mSound)
        mSound = nullptr;
}

}
}
