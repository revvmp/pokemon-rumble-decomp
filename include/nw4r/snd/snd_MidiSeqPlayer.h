#ifndef NW4R_SND_MIDI_SEQ_PLAYER_H
#define NW4R_SND_MIDI_SEQ_PLAYER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_SeqPlayer.h>

namespace nw4r {
namespace snd {
namespace detail {

class MidiSeqPlayer : public SeqPlayer {
private:
    MidiSeqPlayer();

    virtual void ChannelCallback(Channel* pChannel) {}
};

}
}
}

#endif
