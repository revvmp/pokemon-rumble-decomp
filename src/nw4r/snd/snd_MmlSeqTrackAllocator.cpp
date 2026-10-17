#include "nw4r/snd/snd_MmlSeqTrackAllocator.h"

#include "nw4r/snd/snd_InstancePool.h"
#include "nw4r/snd/snd_MmlSeqTrack.h"
#include "nw4r/snd/snd_SeqTrack.h"

namespace nw4r { namespace snd { namespace detail {

SeqTrack *MmlSeqTrackAllocator::AllocTrack(SeqPlayer *player)
{
    MmlSeqTrack *track = mTrackPool.Alloc();
    if (track)
    {
        track->SetSeqPlayer(player);
        track->SetMmlParser(mParser);
    }

    return track;
}

void MmlSeqTrackAllocator::FreeTrack(SeqTrack *track)
{

    track->SetSeqPlayer(nullptr);
    mTrackPool.Free(static_cast<MmlSeqTrack *>(track));
}

u32 MmlSeqTrackAllocator::Create(void *buffer, u32 size)
{
    return mTrackPool.Create(buffer, size);
}

void MmlSeqTrackAllocator::Destroy(void *buffer, u32 size)
{
    mTrackPool.Destroy(buffer, size);
}

}}}
