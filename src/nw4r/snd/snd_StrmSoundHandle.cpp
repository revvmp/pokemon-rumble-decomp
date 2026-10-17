#include "nw4r/snd/snd_StrmSoundHandle.h"







#include "nw4r/snd/snd_StrmSound.h"
#include "nw4r/ut/ut_RuntimeTypeInfo.h"



namespace nw4r { namespace snd {

void StrmSoundHandle::DetachSound()
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
