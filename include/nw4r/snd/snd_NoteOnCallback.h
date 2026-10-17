#ifndef NW4R_SND_NOTE_ON_CALLBACK_H
#define NW4R_SND_NOTE_ON_CALLBACK_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Channel.h>

namespace nw4r {
namespace snd {
namespace detail {

class SeqPlayer;

struct NoteOnInfo {
    int prgNo;
    int key;
    int velocity;
    int length;
    int initPan;
    int priority;
    int voiceOutCount;
    Channel::ChannelCallback channelCallback;
    u32 channelCallbackData;
};

class NoteOnCallback {
public:
    virtual ~NoteOnCallback() {}

    virtual Channel* NoteOn(SeqPlayer* pPlayer, int bankNo,
                            const NoteOnInfo& rInfo) = 0;
};

}
}
}

#endif
