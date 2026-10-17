#ifndef NW4R_SND_SOUND_ARCHIVE_PLAYER_H
#define NW4R_SND_SOUND_ARCHIVE_PLAYER_H

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_BasicSound.h"
#include "nw4r/snd/snd_DisposeCallbackManager.h"
#include "nw4r/snd/snd_MmlParser.h"
#include "nw4r/snd/snd_NoteOnCallback.h"
#include "nw4r/snd/snd_MmlSeqTrackAllocator.h"
#include "nw4r/snd/snd_SeqPlayer.h"
#include "nw4r/snd/snd_SeqSound.h"
#include "nw4r/snd/snd_SoundArchive.h"
#include "nw4r/snd/snd_SoundInstanceManager.h"
#include "nw4r/snd/snd_SoundMemoryAllocatable.h"
#include "nw4r/snd/snd_WsdPlayer.h"
#include "nw4r/snd/snd_SoundStartable.h"
#include "nw4r/snd/snd_StrmChannel.h"
#include "nw4r/snd/snd_StrmSound.h"
#include "nw4r/snd/snd_Util.h"
#include "nw4r/snd/snd_WaveFile.h"
#include "nw4r/snd/snd_WaveSound.h"

namespace nw4r { namespace snd { namespace detail { class PlayerHeap; }}}
namespace nw4r { namespace snd { namespace detail { class SeqTrackAllocator; }}}
namespace nw4r { namespace snd { class SoundActor; }}
namespace nw4r { namespace snd { class SoundHandle; }}
namespace nw4r { namespace snd { class SoundPlayer; }}

namespace nw4r { namespace snd
{

	class SoundArchivePlayer_FileManager
	{

	public:

		virtual void const *GetFileAddress(u32) = 0;
		virtual void const *GetFileWaveDataAddress(u32) = 0;

	private:

	};

	class SoundArchivePlayer : public detail::DisposeCallback,
	                           public SoundStartable
	{

	private:

		struct GroupAddress
		{
			void	const *address;
			void	const *waveDataAddress;
		};

		typedef detail::Util::Table<GroupAddress> GroupAddressTable;

		struct FileAddress
		{
			void	const *address;
			void	const *waveDataAddress;
		};

		typedef detail::Util::Table<FileAddress> FileAddressTable;

		class SeqNoteOnCallback : public detail::NoteOnCallback
		{

		public:

			SeqNoteOnCallback(SoundArchivePlayer const &player) :
				mSoundArchivePlayer(player)
			{
			}

			virtual ~SeqNoteOnCallback() {}

			virtual detail::Channel *NoteOn(
				detail::SeqPlayer *seqPlayer, int bankNo,
				detail::NoteOnInfo const &noteOnInfo);

		private:

			SoundArchivePlayer	const &mSoundArchivePlayer;
		};

		class WsdCallback : public detail::WsdPlayer::WsdCallback
		{

		public:

			WsdCallback(SoundArchivePlayer const &player) :
				mSoundArchivePlayer(player)
			{
			}
			virtual ~WsdCallback() {}

			virtual bool GetWaveSoundData(detail::WaveSoundInfo *info,
			                              detail::WaveSoundNoteInfo *noteInfo,
			                              detail::WaveInfo *waveData,
			                              void const *waveSoundData, int index,
			                              int noteIndex,
			                              u32 userData) const;

		private:

			SoundArchivePlayer	const &mSoundArchivePlayer;
		};

	public:

		SoundArchivePlayer();
		virtual ~SoundArchivePlayer();

		virtual void InvalidateData(void const *start, void const *end);
		virtual void InvalidateWaveData(void const *start, void const *end);

		virtual u32 detail_ConvertLabelStringToSoundId(char const *label)
		{

			return mSoundArchive->ConvertLabelStringToSoundId(label);
		}
		virtual StartResult detail_SetupSound(SoundHandle *handle, u32 soundId,
		                                      bool holdFlag,
		                                      StartInfo const *startInfo);

		bool Setup(SoundArchive const *arc, void *buffer, u32 size,
		           void *strmBuffer, u32 strmBufferSize);
		void Shutdown();

		bool LoadGroup(u32 id, SoundMemoryAllocatable *pAllocatable, u32 blockSize);
		bool LoadGroup(const char *pLabel, SoundMemoryAllocatable *pAllocatable, u32 blockSize);

		bool LoadGroup(int id, SoundMemoryAllocatable *pAllocatable, u32 blockSize) {
			return LoadGroup(static_cast<u32>(id), pAllocatable, blockSize);
		}
		bool LoadGroup(unsigned int id, SoundMemoryAllocatable *pAllocatable, u32 blockSize) {
			return LoadGroup(static_cast<u32>(id), pAllocatable, blockSize);
		}

		bool IsLoadedGroup(u32 groupId) const;

		SoundArchive const &GetSoundArchive() const;
		u32 GetSoundPlayerCount() const { return mSoundPlayerCount; }
		SoundPlayer &GetSoundPlayer(u32 playerId);

		bool IsAvailable() const;
		u32 GetRequiredMemSize(SoundArchive const *arc);
		u32 GetRequiredStrmBufferSize(SoundArchive const *arc);
		void const *GetGroupAddress(u32 groupId) const;
		void SetGroupAddress(u32 id, const void* pAddr);
		void const *GetGroupWaveDataAddress(u32 groupId) const;
		void SetGroupWaveDataAddress(u32 id, const void* pAddr);
		void const *GetFileAddress(u32 fileId) const;
		void const *GetFileWaveDataAddress(u32 fileId) const;
		void const *detail_GetFileAddress(u32 fileId) const;
		void const *detail_GetFileWaveDataAddress(u32 fileId) const;

		bool SetupMram(SoundArchive const *arc, void *buffer, u32 size);
		bool SetupStrmBuffer(SoundArchive const *arc, void *buffer, u32 size);
		bool SetupSoundPlayer(SoundArchive const *arc, void **buffer,
		                      void *endp);

		bool SetupSeqSound(SoundArchive const *arc, int numSounds,
		                   void **buffer, void *endp);
		bool SetupWaveSound(SoundArchive const *arc, int numSounds,
		                    void **buffer, void *endp);
		bool SetupStrmSound(SoundArchive const *arc, int numSounds,
		                    void **buffer, void *endp);
		bool SetupSeqTrack(SoundArchive const *arc, int numTracks,
		                   void **buffer, void *endp);

		bool CreateGroupAddressTable(SoundArchive const *arc, void **buffer,
		                             void *endp);
		bool CreateFileAddressTable(SoundArchive const *arc, void **buffer,
		                            void *endp);

		detail::PlayerHeap *CreatePlayerHeap(void **buffer, void *endp,
		                                     u32 heapSize);

		void Update();
		void UpdateCommonSoundParam(detail::BasicSound *sound,
		                            SoundArchive::SoundInfo const *commonInfo);

		SoundStartable::StartResult detail_SetupSoundImpl(
			SoundHandle *handle, u32 soundId,
			detail::BasicSound::AmbientInfo *ambientArgInfo, SoundActor *actor,
			bool holdFlag, SoundStartable::StartInfo const *startInfo);

	private:
		template <typename Sound>
		Sound *AllocSound(
			detail::SoundInstanceManager<Sound> *manager, u32 soundId,
			int priority, int ambientPriority,
			detail::BasicSound::AmbientInfo *ambientArgInfo);

		SoundStartable::StartResult PrepareSeqImpl(
			detail::SeqSound *sound, SoundArchive::SoundInfo const *commonInfo,
			SoundArchive::SeqSoundInfo const *info,
			SoundStartable::StartInfo::StartOffsetType startOffsetType,
			int startOffset, void const *externalSeqDataAddress,
			char const *externalSeqStartLabel);
		SoundStartable::StartResult PrepareStrmImpl(
			detail::StrmSound *sound, SoundArchive::SoundInfo const *commonInfo,
			SoundArchive::StrmSoundInfo const *info,
			SoundStartable::StartInfo::StartOffsetType startOffsetType,
			int startOffset);
		SoundStartable::StartResult PrepareWaveSoundImpl(
			detail::WaveSound *sound, SoundArchive::SoundInfo const *commonInfo,
			SoundArchive::WaveSoundInfo const *info,
			SoundStartable::StartInfo::StartOffsetType startOffsetType,
			int startOffset);

	public:
		static int const DEFAULT_STREAM_BLOCK_COUNT;

	private:

		SoundArchive									const *mSoundArchive;
		GroupAddressTable								*mGroupTable;
		FileAddressTable								*mFileTable;
		SoundArchivePlayer_FileManager					*mFileManager;
		SeqNoteOnCallback								mSeqCallback;
		WsdCallback										mWsdCallback;
		detail::SeqTrackAllocator						*mSeqTrackAllocator;
		detail::SeqPlayer::SeqUserprocCallback			*mSeqUserprocCallback;
		void											*mSeqUserprocCallbackArg;
		u32												mSoundPlayerCount;
		SoundPlayer										*mSoundPlayers;
		detail::SoundInstanceManager<detail::SeqSound>	mSeqSoundInstanceManager;
		detail::SoundInstanceManager<detail::StrmSound>	mStrmSoundInstanceManager;
		detail::SoundInstanceManager<detail::WaveSound>	mWaveSoundInstanceManager;
		detail::MmlSeqTrackAllocator					mMmlSeqTrackAllocator;
		detail::StrmBufferPool							mStrmBufferPool;
		detail::MmlParser								mMmlParser;
		void											*mSetupBufferAddress;
		u32												mSetupBufferSize;
	};
}}

#endif
