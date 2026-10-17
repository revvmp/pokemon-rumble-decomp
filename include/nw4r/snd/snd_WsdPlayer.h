#ifndef NW4R_SND_WSD_PLAYER_H
#define NW4R_SND_WSD_PLAYER_H

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_BasicPlayer.h"
#include "nw4r/snd/snd_Channel.h"
#include "nw4r/snd/snd_DisposeCallbackManager.h"
#include "nw4r/snd/snd_Lfo.h"
#include "nw4r/snd/snd_SoundThread.h"
#include "nw4r/snd/snd_WaveFile.h"
#include "nw4r/snd/snd_WsdFile.h"

namespace nw4r { namespace snd {

struct WsdDataInfo {
	  bool loopFlag;
	  int sampleRate;
	  u32 loopStart;
	  u32 loopEnd;
};

namespace detail
{
	class WsdPlayer : public BasicPlayer,
	                  public DisposeCallback,
	                  public SoundThread::PlayerCallback
	{
	public:
		enum StartOffsetType
		{
			START_OFFSET_TYPE_SAMPLE,
			START_OFFSET_TYPE_MILLISEC,
		};

	public:
		class WsdCallback
		{
		public:
			virtual ~WsdCallback() {}

			virtual bool GetWaveSoundData(WaveSoundInfo *info,
			                              WaveSoundNoteInfo *noteInfo,
			                              WaveInfo *waveData,
			                              void const *waveSoundData, int index,
			                              int noteIndex,
			                              u32 userData) const = 0;

		private:
		};

	public:
		WsdPlayer();

		virtual bool Start();
		virtual void Stop();
		virtual void Pause(bool flag);
		virtual bool IsActive() const { return mActiveFlag != false; }
		virtual bool IsStarted() const { return mStartedFlag != false; }
		virtual bool IsPause() const { return mPauseFlag != false; }

		virtual void InvalidateData(void const *start, void const *end);
		virtual void InvalidateWaveData(void const *, void const *) {}

		virtual void OnUpdateFrameSoundThread() { Update(); }
		virtual void OnShutdownSoundThread() { Stop(); }

		bool Prepare(void const *waveSoundBase, int index,
		             StartOffsetType startOffsetType, int startOffset,
		             int voiceOutCount, WsdCallback const *callback,
		             u32 callbackData);

		f32 GetPanRange() const { return mPanRange; }
		int GetVoiceOutCount() const { return mVoiceOutCount; }
		int GetChannelPriority() const { return mPriority; }
		void const *GetWsdDataAddress() const { return mWsdData; }

		void SetReleasePriorityFix(bool flag);
		void SetChannelPriority(int priority);

		bool IsChannelActive() const
		{
			return mChannel != nullptr && mChannel->IsActive();
		}
		bool ReadWsdDataInfo(WsdDataInfo*) const;
		u32 GetPlaySamplePosition() const;

	private:
		static void ChannelCallbackFunc(Channel *dropChannel,
		                                Channel::ChannelCallbackStatus status,
		                                u32 userData);

		void InitParam(int voiceOutCount, WsdCallback const *callback,
		               u32 callbackData);

		void Update();

		bool StartChannel(WsdCallback const *callback, u32 callbackData);
		void UpdateChannel();
		void CloseChannel();

		void FinishPlayer();

	public:
		static int const DEFAULT_PRIORITY = 64;
		static int const MUTE_RELEASE_VALUE;
		static int const PAUSE_RELEASE_VALUE;

	private:
		bool			mActiveFlag;
		bool			mStartedFlag;
		bool			mPauseFlag;
		bool			mWavePlayFlag;
		bool			mReleasePriorityFixFlag;
		f32				mPanRange;
		int				mVoiceOutCount;
		u8				mPriority;
		WsdCallback		const *mCallback;
		u32		mCallbackData;
		void			const *mWsdData;
		int				mWsdIndex;
		StartOffsetType	mStartOffsetType;
		int				mStartOffset;
		LfoParam		mLfoParam;
		WaveSoundInfo	mWaveSoundInfo;
		Channel			*mChannel;
	};
}}}

#endif
