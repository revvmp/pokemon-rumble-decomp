#include "nw4r/snd/snd_WaveSound.h"

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_BasicSound.h"
#include "nw4r/snd/snd_SoundInstanceManager.h"
#include "nw4r/snd/snd_WaveSoundHandle.h"
#include "nw4r/snd/snd_WsdPlayer.h"

#include "nw4r/ut/ut_RuntimeTypeInfo.h"

namespace nw4r { namespace snd { namespace detail
{
	ut::detail::RuntimeTypeInfo const WaveSound::typeInfo(
		&BasicSound::typeInfo);
}}}

namespace nw4r { namespace snd { namespace detail {

WaveSound::WaveSound(SoundInstanceManager<WaveSound> *manager, int priority,
                     int ambientPriority) :
	BasicSound			(priority, ambientPriority),
	mTempSpecialHandle	(nullptr),
	mManager			(manager),
	mPreparedFlag		(false)
{
}

bool WaveSound::Prepare(void const *waveSoundBase, s32 waveSoundOffset,
                        WsdPlayer::StartOffsetType startOffsetType, s32 offset,
                        WsdPlayer::WsdCallback const *callback,
                        u32 callbackData)
{

	InitParam();

	bool result =
		mWsdPlayer.Prepare(waveSoundBase, waveSoundOffset, startOffsetType,
	                       offset, GetVoiceOutCount(), callback, callbackData);
	if (!result)
		return false;

	mPreparedFlag = true;
	return true;
}

void WaveSound::Shutdown()
{
	BasicSound::Shutdown();

	mManager->Free(this);
}

void WaveSound::SetChannelPriority(int priority)
{

	mWsdPlayer.SetChannelPriority(priority);
}

void WaveSound::SetReleasePriorityFix(bool flag)
{
	mWsdPlayer.SetReleasePriorityFix(flag);
}

void WaveSound::OnUpdatePlayerPriority()
{
	mManager->UpdatePriority(this, CalcCurrentPlayerPriority());
}

bool WaveSound::IsAttachedTempSpecialHandle()
{
	return mTempSpecialHandle != nullptr;
}

void WaveSound::DetachTempSpecialHandle()
{
	mTempSpecialHandle->DetachSound();
}

}}}
