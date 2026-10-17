#ifndef NW4R_SND_STRM_SOUND_H
#define NW4R_SND_STRM_SOUND_H



#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_BasicSound.h"
#include "nw4r/snd/snd_debug.h"
#include "nw4r/snd/snd_MoveValue.h"
#include "nw4r/snd/snd_StrmPlayer.h"

#include "nw4r/ut/ut_LinkList.h"




namespace nw4r { namespace snd { namespace detail { template <class> class SoundInstanceManager; }}}
namespace nw4r { namespace snd { namespace detail { class StrmBufferPool; }}}
namespace nw4r { namespace snd { class StrmSoundHandle; }}

namespace nw4r { namespace ut { namespace detail { class RuntimeTypeInfo; }}}
namespace nw4r { namespace ut { class FileStream; }}



namespace nw4r { namespace snd { namespace detail
{

	class StrmSound : public BasicSound
	{

	public:

		typedef ut::LinkList<StrmSound, 0xf0> PriorityLinkList;


	public:

		StrmSound(SoundInstanceManager<StrmSound> *manager, int priority,
		          int ambientPriority);




		virtual ut::detail::RuntimeTypeInfo const *GetRuntimeTypeInfo() const
		{
			return &typeInfo;
		}
		virtual ~StrmSound() {}
		virtual void Shutdown();
		virtual bool IsPrepared() const { return mStrmPlayer.IsPrepared(); }
		virtual bool IsAttachedTempSpecialHandle();
		virtual void DetachTempSpecialHandle();
		virtual void InitParam();
		virtual BasicPlayer &GetBasicPlayer() { return mStrmPlayer; }
		virtual BasicPlayer const &GetBasicPlayer() const
		{
			return mStrmPlayer;
		}
		virtual void OnUpdatePlayerPriority();
		virtual void UpdateMoveValue();
		virtual void UpdateParam();


		StrmPlayer::SetupResult Setup(StrmBufferPool *bufferPool,
		                              int allocChannelCount,
		                              u16 allocTrackFlag);

		void *GetFileStreamBuffer() { return mFileStreamBuffer; }
		s32 GetFileStreamBufferSize() { return sizeof mFileStreamBuffer; }

		bool Prepare(StrmPlayer::StartOffsetType startOffsetType, s32 offset,
		             ut::FileStream *fileStream);

		void SetTrackVolume(u32 trackFlags, f32 volume, int fadeFrames);



		u32 GetPlaySamplePosition() const { return mStrmPlayer.GetPlaySamplePosition(); }


	public:
		static int const FILE_STREAM_BUFFER_SIZE = 128;

		static ut::detail::RuntimeTypeInfo const typeInfo;


	private:

		StrmPlayer						mStrmPlayer;
		StrmSoundHandle					*mTempSpecialHandle;
		SoundInstanceManager<StrmSound>	*mManager;
		MoveValue<f32, int>				mTrackVolume[8];
		ut::FileStream					*mFileStream;
		int								mFileStreamBuffer[FILE_STREAM_BUFFER_SIZE];


	private:
		friend class snd::StrmSoundHandle;
	};
}}}

#endif
