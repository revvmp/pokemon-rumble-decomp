#ifndef NW4R_SND_STRM_CHANNEL_H
#define NW4R_SND_STRM_CHANNEL_H





#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_adpcm.h"
#include <nw4r/snd/snd_Types.h>



namespace nw4r { namespace snd { namespace detail
{
class StrmBufferPool {
public:
    void Setup(void* pBase, u32 size, int count);
    void Shutdown();

    void* Alloc();
    void Free(void* pBuffer);

    u32 GetBlockSize() const {
        return mBlockSize;
    }

private:
    static const int BLOCK_MAX = 32;
    static const int BITS_PER_BYTE = 8;

private:
    void* mBuffer;
    u32 mBufferSize;

    u32 mBlockSize;
    int mBlockCount;

    int mAllocCount;
    u8 mAllocFlags[BLOCK_MAX / BITS_PER_BYTE];
};

	struct StrmChannel
	{
		void			*bufferAddress;
		AdpcmParam		adpcmParam;
		AdpcmLoopParam	adpcmLoopParam;
		u16				adpcmPredScale;
	};
}}}



#endif
