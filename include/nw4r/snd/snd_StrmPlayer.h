#ifndef NW4R_SND_STRM_PLAYER_H
#define NW4R_SND_STRM_PLAYER_H



#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_BasicPlayer.h"
#include <nw4r/snd/snd_Types.h>
#include "nw4r/snd/snd_InstancePool.h"
#include "nw4r/snd/snd_Task.h"
#include "nw4r/snd/snd_SoundThread.h"
#include "nw4r/snd/snd_StrmChannel.h"
#include "nw4r/snd/snd_StrmFile.h"
#include "nw4r/snd/snd_Voice.h"

#include "nw4r/ut/ut_LinkList.h"

#include <revolution/os/OSMutex.h>




namespace nw4r { namespace ut { class FileStream; }}



namespace nw4r { namespace snd {

struct StrmDataInfo
{
	 bool loopFlag;
	 int sampleRate;
	 u32 loopStart;
	 u32 loopEnd;
};

namespace detail
{

	class StrmPlayer : public BasicPlayer, public SoundThread::PlayerCallback
	{

	public:

		enum StartOffsetType
		{
			START_OFFSET_TYPE_SAMPLE,
			START_OFFSET_TYPE_MILLISEC,
		};


		enum SetupResult
		{
			SETUP_SUCCESS,

			SETUP_ERR_CANNOT_ALLOCATE_BUFFER,

			SETUP_ERR_UNKNOWN
		};


	public:

		struct StrmTrack
		{
			bool							mActiveFlag;
			Voice							*mVoice;
			StrmFileReader::StrmTrackInfo	mTrackInfo;
			f32								mVolume;
			f32								mPan;
		};


		class StrmHeaderLoadTask : public Task
		{

		public:

			StrmHeaderLoadTask();



			virtual void Execute();
			virtual void Cancel();
			virtual void OnCancel();


		public:

			StrmPlayer		*player;
			ut::FileStream	*fileStream;
			StartOffsetType	startOffsetType;
			s32				startOffset;
		};


		class StrmDataLoadTask : public Task
		{

		public:
			typedef ut::LinkList<StrmDataLoadTask, 0x2c> LinkList;


		public:

			StrmDataLoadTask();



			virtual void Execute();
			virtual void Cancel();
			virtual void OnCancel();


		public:

			StrmPlayer			*mStrmPlayer;
			ut::FileStream		*fileStream;
			u32					mSize;
			s32					mOffset;
			u32					mBlockSize;
			s32					mBufferBlockIndex;
			bool				mNeedUpdateAdpcmLoop;

			ut::LinkListNode	link;
		};


	public:
		StrmPlayer();
		virtual ~StrmPlayer();



		virtual bool Start();
		virtual void Stop();
		virtual void Pause(bool flag);
		virtual bool IsActive() const { return mActiveFlag; }
		virtual bool IsStarted() const { return mStartedFlag; }
		virtual bool IsPause() const { return mPauseFlag; }


		virtual void OnUpdateFrameSoundThread() { Update(); }
		virtual void OnUpdateVoiceSoundThread() { UpdateBuffer(); }
		virtual void OnShutdownSoundThread() { Stop(); }


		SetupResult Setup(StrmBufferPool *bufferPool, int allocChannelCount,
		                  u16 allocTrackFlag, int voiceOutCount);
		void Update();
		void Shutdown();

		bool IsPrepared() const { return mPreparedFlag; }
		StrmChannel *GetTrackChannel(StrmTrack const &track, int channelIndex);
		StrmTrack *GetPlayerTrack(int trackNo);

		void SetTaskErrorFlag() { mTaskErrorFlag = true; }
		void SetTaskCancelFlag() { mTaskCancelFlag = true; }

		bool IsAdpcm() const
		{
			return mStrmInfo.sampleFormat == SAMPLE_FORMAT_DSP_ADPCM;
		}
		bool CheckDiskDriveError() const;

		void SetAdpcmLoopContext(int channelNum, u16 *predScale);
		void SetTrackVolume(u32 trackBitFlag, f32 volume);
		u32 GetPlaySamplePosition() const;

		bool ReadStrmDataInfo(StrmDataInfo*) const;
		void InitParam();
		bool SetupPlayer();
		bool Prepare(ut::FileStream *fileStream,
		             StartOffsetType startOffsetType, int startOffset);

		bool LoadHeader(ut::FileStream *fileStream,
		                StartOffsetType startOffsetType, int startOffset);
		bool LoadStreamData(ut::FileStream *fileStream, int offset, u32 size,
		                    u32 blockSize, int bufferBlockIndex,
		                    bool needUpdateAdpcmLoop);

		bool AllocStrmBuffers();
		void FreeStrmBuffers();

		bool AllocVoices(int voiceOutCount);
		void FreeVoices();

		int CalcLoadingBufferBlockCount() const;
		bool CalcStartOffset(s32 *startBlockIndex, u32 *startBlockOffset,
		                     s32 *loopCount);

		void SetLoopEndToZeroBuffer(int endBufferBlockIndex);

		void UpdateBuffer();
		void UpdatePauseStatus();
		void UpdateLoadingBlockIndex();
		void UpdatePlayingBlockIndex();
		void UpdateLoopAddress(u32 loopStartSamples, u32 loopEndSamples);
		void UpdateDataLoopAddress(s32 endBlockBufferIndex);
		void UpdateVoiceParams(StrmTrack *track);

		static u32 GetSampleByByte(u32 byte, SampleFormat format);

	private:
		static void VoiceCallbackFunc(Voice *voice,
		                              Voice::VoiceCallbackStatus status,
		                              void *arg);


	public:
		static int const LOAD_BUFFER_SIZE = 0x4000;
		static int const LOAD_BUFFER_CHANNEL_NUM;
		static int const BUFFER_BLOCK_COUNT_MAX = 32;
		static int const STRM_CHANNEL_NUM_PER_TRACK;
		static int const STRM_CHANNEL_NUM = 16;
		static int const DATA_BLOCK_SIZE_MAX = 0x2000;
		static int const STRM_TRACK_NUM = 8;

	private:
		static bool sStaticInitFlag;
		static OSMutex sLoadBufferMutex;
		static u8 sLoadBuffer[LOAD_BUFFER_SIZE];


	private:


		StrmFileReader::StrmInfo		mStrmInfo;
		bool							volatile mSetupFlag;
		bool							mActiveFlag;
		bool							mStartedFlag;
		bool							mPreparedFlag;
		bool							mTaskErrorFlag;
		bool							mTaskCancelFlag;
		bool							mLoadingDelayFlag;
		bool							mPauseFlag;
		bool							mPauseStatus;
		bool							mLoadWaitFlag;
		bool							mNoRealtimeLoadFlag;
		bool							mSkipUpdateAdpcmLoop;
		bool							mValidAdpcmLoop;
		bool							mPlayFinishFlag;
		bool							mLoadFinishFlag;
		bool							mAllocStrmBufferFlag;
		s32								mLoopCounter;
		int								mPrepareCounter;
		int								mChangeNumBlocks;
		int								mDataBlockSize;
		int								mBufferBlockCount;
		int								mBufferBlockCountBase;
		int								mLoadingBufferBlockCount;
		int								mLoadingBufferBlockIndex;
		int								mLoadingDataBlockIndex;
		int								mPlayingBufferBlockCount;
		int								mPlayingBufferBlockIndex;
		int								mPlayingDataBlockIndex;
		int								mLoopStartBlockIndex;
		int								mLastBlockIndex;
		int								mLoadWaitCount;
		StartOffsetType					mStartOffsetType;
		int								mStartOffset;
		StrmHeaderLoadTask				mStrmHeaderLoadTask;
		StrmDataLoadTask::LinkList		mStrmDataLoadTaskList;
		InstancePool<StrmDataLoadTask>	mStrmDataLoadTaskPool;
		StrmDataLoadTask				mStrmDataLoadTaskArea[BUFFER_BLOCK_COUNT_MAX];
		StrmBufferPool					*mBufferPool;
		ut::FileStream					*mFileStream;
		s32								mTrackCount;
		s32								mChannelCount;
		s32								mVoiceOutCount;
		StrmChannel						mChannels[STRM_CHANNEL_NUM];
		StrmTrack						mTracks[STRM_TRACK_NUM];
	};
}}}

#endif
