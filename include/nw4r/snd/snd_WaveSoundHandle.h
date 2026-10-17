#ifndef NW4R_SND_WAVE_SOUND_HANDLE_H
#define NW4R_SND_WAVE_SOUND_HANDLE_H

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_WaveSound.h"

#include "nw4r/ut/ut_NonCopyable.h"

namespace nw4r { namespace snd { namespace detail { class WaveSound; }}}
namespace nw4r { namespace snd { class SoundHandle; }}

namespace nw4r { namespace snd
{
	class WaveSoundHandle : private ut::NonCopyable
	{
	public:
		WaveSoundHandle(SoundHandle *handle);
		~WaveSoundHandle() { DetachSound(); }

		bool IsAttachedSound() const { return mSound != nullptr; }

		u32 GetPlaySamplePosition() const {
			if (!IsAttachedSound())
				return -1;
			return mSound->GetPlaySamplePosition();
		}

		void DetachSound();

	private:
		detail::WaveSound	*mSound;
	};
}}

#endif
