#ifndef NW4R_SND_STRM_SOUND_HANDLE_H
#define NW4R_SND_STRM_SOUND_HANDLE_H



#include <nw4r/types_nw4r.h>


#include "nw4r/snd/snd_StrmSound.h"
#include "nw4r/ut/ut_NonCopyable.h"




namespace nw4r { namespace snd { namespace detail { class StrmSound; }}}



namespace nw4r { namespace snd
{

	class StrmSoundHandle : private ut::NonCopyable
	{

	public:

		StrmSoundHandle(SoundHandle*);
		~StrmSoundHandle() { DetachSound(); }
		bool IsAttachedSound() const { return mSound != nullptr; }

		void DetachSound();

		void SetVolume(u32 trackFlags, f32 volume, int fadeFrames) {
			if (IsAttachedSound())
				mSound->SetTrackVolume(trackFlags, volume, fadeFrames);
		}

		u32 GetPlaySamplePosition() const {
			if (!IsAttachedSound())
				return -1;
			return mSound->GetPlaySamplePosition();
		}


	private:

		detail::StrmSound	*mSound;
	};
}}

#endif
