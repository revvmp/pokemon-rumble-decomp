#ifndef NW4R_SND_MML_SEQ_TRACK_H
#define NW4R_SND_MML_SEQ_TRACK_H

#include "nw4r/snd/snd_SeqTrack.h"

namespace nw4r { namespace snd { namespace detail { class MmlParser; }}}

namespace nw4r { namespace snd { namespace detail
{
    class MmlSeqTrack : public SeqTrack
    {
    public:
        MmlSeqTrack();

        virtual ParseResult Parse(bool doNoteOn);

        void SetMmlParser(MmlParser const *parser) { mParser = parser; }
    private:
        MmlParser const *mParser;
    };
}}}

#endif
