#ifndef NW4R_SND_MML_SEQ_TRACK_ALLOCATOR_H
#define NW4R_SND_MML_SEQ_TRACK_ALLOCATOR_H
#include <new>

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_InstancePool.h"
#include "nw4r/snd/snd_SeqTrackAllocator.h"

namespace nw4r { namespace snd { namespace detail { class MmlParser; }}}
namespace nw4r { namespace snd { namespace detail { class MmlSeqTrack; }}}

namespace nw4r { namespace snd { namespace detail
{
    class MmlSeqTrackAllocator : public SeqTrackAllocator
    {
    public:
        MmlSeqTrackAllocator(MmlParser *parser) : mParser(parser) {}

        virtual SeqTrack *AllocTrack(SeqPlayer *player);
        virtual void FreeTrack(SeqTrack *track);
        virtual int GetAllocatableTrackCount() const
        {
            return mTrackPool.Count();
        }

        u32 Create(void *buffer, u32 size);
        void Destroy(void *buffer, u32 size);
    private:
        MmlParser *mParser;
        InstancePool<MmlSeqTrack> mTrackPool;
    };
}}}

#endif
