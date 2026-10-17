#ifndef NW4R_SND_WAVE_SOUND_H
#define NW4R_SND_WAVE_SOUND_H

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_BasicSound.h"
#include "nw4r/snd/snd_debug.h"
#include "nw4r/snd/snd_WsdPlayer.h"

#include "nw4r/ut/ut_LinkList.h"

namespace nw4r { namespace snd { namespace detail { template <class> class SoundInstanceManager; }}}
namespace nw4r { namespace snd { class WaveSoundHandle; }}

namespace nw4r { namespace ut { namespace detail { class RuntimeTypeInfo; }}}

namespace nw4r { namespace snd { namespace detail
{
	class WaveSound : public BasicSound
	{
	public:
		typedef ut::LinkList<WaveSound, 0xf0> PriorityLinkList;

	public:
		WaveSound(SoundInstanceManager<WaveSound> *manager, int priority,
		          int ambientPriority);

		virtual ut::detail::RuntimeTypeInfo const *GetRuntimeTypeInfo() const
		{
			return &typeInfo;
		}
		virtual ~WaveSound() {}
		virtual void Shutdown();
		virtual bool IsPrepared() const { return mPreparedFlag; }
		virtual bool IsAttachedTempSpecialHandle();
		virtual void DetachTempSpecialHandle();
		virtual BasicPlayer &GetBasicPlayer() { return mWsdPlayer; }
		virtual BasicPlayer const &GetBasicPlayer() const { return mWsdPlayer; }
		virtual void OnUpdatePlayerPriority();

		bool Prepare(void const *waveSoundBase, s32 waveSoundOffset,
		             WsdPlayer::StartOffsetType startOffsetType, s32 offset,
		             WsdPlayer::WsdCallback const *callback,
		             u32 callbackData);

		void SetChannelPriority(int priority);
		void SetReleasePriorityFix(bool flag);

		u32 GetPlaySamplePosition() const { return mWsdPlayer.GetPlaySamplePosition(); }

	public:
		static ut::detail::RuntimeTypeInfo const typeInfo;

	private:
		WsdPlayer						mWsdPlayer;
		WaveSoundHandle					*mTempSpecialHandle;
		SoundInstanceManager<WaveSound>	*mManager;
		bool							mPreparedFlag;

	private:
		friend class snd::WaveSoundHandle;
	};
}}}

#endif
