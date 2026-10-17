#ifndef NW4R_SND_SEQ_TRACK_ALLOCATOR_H
#define NW4R_SND_SEQ_TRACK_ALLOCATOR_H

namespace nw4r { namespace snd { namespace detail { class SeqPlayer; }}}
namespace nw4r { namespace snd { namespace detail { class SeqTrack; }}}

namespace nw4r { namespace snd { namespace detail
{
    class SeqTrackAllocator
    {
    public:
        virtual ~SeqTrackAllocator() {}

        virtual SeqTrack *AllocTrack(SeqPlayer *player) = 0;
        virtual void FreeTrack(SeqTrack *track) = 0;
        virtual int GetAllocatableTrackCount() const = 0;
    private:
    };
}}}

#endif
