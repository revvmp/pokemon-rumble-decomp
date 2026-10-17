#ifndef NW4R_SND_PLAYER_HEAP_H
#define NW4R_SND_PLAYER_HEAP_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_SoundMemoryAllocatable.h>

#include <nw4r/ut/ut_LinkList.h>

namespace nw4r {
namespace snd {

class SoundPlayer;

namespace detail {

class BasicSound;

class PlayerHeap : public SoundMemoryAllocatable {
public:
    PlayerHeap();
    virtual ~PlayerHeap();

    virtual void* Alloc(u32 size);

    bool Create(void* pStartAddress, u32 size);
    void Destroy();
    void Clear();
    u32 GetFreeSize() const;

    void AttachSound(BasicSound* pSound);
    void DetachSound(BasicSound* pSound);

    void AttachSoundPlayer(SoundPlayer* pPlayer) {
        mPlayer = pPlayer;
    }

private:
    BasicSound* mSound;
    SoundPlayer* mPlayer;
    void* mStartAddress;
    void* mEndAddress;
    void* mAllocAddress;

public:
    NW4R_UT_LINKLIST_NODE_DECL();
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(PlayerHeap);

}
}
}

#endif
