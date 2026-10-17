#include "nw4r/snd/snd_WaveSoundHandle.h"





#include <nw4r/types_nw4r.h> 

#include "nw4r/snd/snd_BasicSound.h"
#include "nw4r/snd/snd_SoundHandle.h"
#include "nw4r/snd/snd_WaveSound.h"

#include "nw4r/ut/ut_RuntimeTypeInfo.h" 





namespace nw4r { namespace snd {

WaveSoundHandle::WaveSoundHandle(SoundHandle *handle) :
	mSound	(nullptr)
{
	if (!handle)
		return;

	detail::BasicSound *basicSound = handle->detail_GetAttachedSound();
	if (!basicSound)
		return;

	if (detail::WaveSound *sound =
	        ut::DynamicCast<detail::WaveSound *>(basicSound))
	{ 

		mSound = sound;

		if (mSound->IsAttachedTempSpecialHandle())
			mSound->DetachTempSpecialHandle();

		mSound->mTempSpecialHandle = this;
	}
}

void WaveSoundHandle::DetachSound()
{
	if (IsAttachedSound())
	{
		if (mSound->mTempSpecialHandle == this)
			mSound->mTempSpecialHandle = nullptr;
	}

	if (mSound)
		mSound = nullptr;
}

}} 
