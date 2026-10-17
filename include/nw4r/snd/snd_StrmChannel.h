#ifndef NW4R_SND_STRM_CHANNEL_H
#define NW4R_SND_STRM_CHANNEL_H





#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_adpcm.h"
#include "nw4r/snd/snd_Types.h"



namespace nw4r { namespace snd { namespace detail
{
class StrmBufferPool;

	struct StrmChannel
	{
		void			*bufferAddress;
		AdpcmParam		adpcmParam;
		AdpcmLoopParam	adpcmLoopParam;
		u16				adpcmPredScale;
	};
}}}



#endif
