#include "nw4r/snd/snd_SoundPlayer.h"

#include <limits.h>

#include "nw4r/snd/snd_BasicSound.h"
#include "nw4r/snd/snd_Types.h"
#include "nw4r/snd/snd_PlayerHeap.h"
#include "nw4r/snd/snd_SoundThread.h"

#include "nw4r/ut/ut_algorithm.h"

namespace nw4r { namespace snd {

SoundPlayer::SoundPlayer() :
	mPlayableCount		(1),
	mPlayableLimit		(INT_MAX),
	mVolume				(1.0f),
	mLpfFreq			(0.0f),
	mOutputLineFlag		(OUTPUT_LINE_MAIN),
	mMainOutVolume		(1.0f),
	mBiquadType			(0),
	mBiquadValue		(0.0f),
	mMainSend			(0.0f)
{
	for (int i = 0; i < 4; i++)
		mRemoteOutVolume[i] = 1.0f;

	for (int i = 0; i < AUX_BUS_NUM; i++)
		mFxSend[i] = 0.0f;

}

SoundPlayer::~SoundPlayer()
{
	StopAllSound(0);
}

void SoundPlayer::Update()
{
	detail::SoundThread::AutoLock lock;

	for (detail::BasicSoundPlayerPlayList::Iterator itr = mSoundList.GetBeginIter(); itr != mSoundList.GetEndIter();)
	{
		detail::BasicSoundPlayerPlayList::Iterator curItr = itr++;

		curItr->Update();
	}

	detail_SortPriorityList();
}

void SoundPlayer::StopAllSound(int fadeFrames)
{
	detail::SoundThread::AutoLock lock;

	for (detail::BasicSoundPlayerPlayList::Iterator itr = mSoundList.GetBeginIter(); itr != mSoundList.GetEndIter();)
	{
		detail::BasicSoundPlayerPlayList::Iterator curItr = itr++;

		curItr->Stop(fadeFrames);
	}
}

void SoundPlayer::PauseAllSound(bool flag, int fadeFrames)
{
	detail::SoundThread::AutoLock lock;

	for (detail::BasicSoundPlayerPlayList::Iterator itr = mSoundList.GetBeginIter(); itr != mSoundList.GetEndIter();)
	{
		detail::BasicSoundPlayerPlayList::Iterator curItr = itr++;

		curItr->Pause(flag, fadeFrames);
	}
}

void SoundPlayer::SetVolume(f32 volume)
{
	if (volume < 0.0f)
		volume = 0.0f;

	mVolume = volume;
}

void SoundPlayer::SetLpfFreq(f32 freq)
{
	mLpfFreq = freq;
}

f32 SoundPlayer::GetRemoteOutVolume(int remote) const
{
	return mRemoteOutVolume[remote];
}

void SoundPlayer::SetFxSend(AuxBus bus, f32 send)
{
	mFxSend[bus] = send;
}

void SoundPlayer::RemoveSoundList(detail::BasicSound *sound)
{
	detail::SoundThread::AutoLock lock;

	mSoundList.Erase(sound);
	sound->DetachSoundPlayer(this);
}

void SoundPlayer::InsertPriorityList(detail::BasicSound *sound)
{
	detail::SoundThread::AutoLock lock;

	detail::BasicSoundPlayerPrioList::Iterator itr = mPriorityList.GetBeginIter();
	for (; itr != mPriorityList.GetEndIter(); ++itr)
	{
		if (sound->CalcCurrentPlayerPriority()
		    < itr->CalcCurrentPlayerPriority())
		{
			break;
		}
	}

	mPriorityList.Insert(itr, sound);
}

void SoundPlayer::RemovePriorityList(detail::BasicSound *sound)
{
	detail::SoundThread::AutoLock lock;

	mPriorityList.Erase(sound);
}

void SoundPlayer::detail_SortPriorityList(detail::BasicSound *sound)
{
	RemovePriorityList(sound);
	InsertPriorityList(sound);
}

void SoundPlayer::detail_SortPriorityList()
{
	detail::SoundThread::AutoLock lock;

	if (mPriorityList.GetSize() < 2)
		return;

	static detail::BasicSoundPlayerPrioList
		tmplist[detail::BasicSound::PRIORITY_MAX + 1];

	while (!mPriorityList.IsEmpty())
	{
		detail::BasicSound &front = mPriorityList.GetFront();
		mPriorityList.PopFront();
		tmplist[front.CalcCurrentPlayerPriority()].PushBack(&front);
	}

	for (int i = 0; i < (int)(sizeof(tmplist) / sizeof(tmplist[0])); i++)
	{
		while (!tmplist[i].IsEmpty())
		{
			detail::BasicSound &front = tmplist[i].GetFront();

			tmplist[i].PopFront();
			mPriorityList.PushBack(&front);
		}
	}
}

bool SoundPlayer::detail_AppendSound(detail::BasicSound *sound)
{

	detail::SoundThread::AutoLock lock;

	int allocPriority = sound->CalcCurrentPlayerPriority();

	if (GetPlayableSoundCount() == 0)
		return false;

	while (GetPlayingSoundCount() >= GetPlayableSoundCount())
	{
		detail::BasicSound *dropSound = GetLowestPrioritySound();
		if (!dropSound)
			return false;

		if (allocPriority < dropSound->CalcCurrentPlayerPriority())
			return false;

		dropSound->Shutdown();
	}

	mSoundList.PushBack(sound);
	InsertPriorityList(sound);
	sound->AttachSoundPlayer(this);

	return true;
}

void SoundPlayer::detail_RemoveSound(detail::BasicSound *sound)
{
	RemovePriorityList(sound);
	RemoveSoundList(sound);
}

void SoundPlayer::SetPlayableSoundCount(int count)
{

	detail::SoundThread::AutoLock lock;

	mPlayableCount = ut::Clamp(count, 0, mPlayableLimit);

	while (GetPlayingSoundCount() > GetPlayableSoundCount())
	{
		detail::BasicSound *dropSound = GetLowestPrioritySound();

		dropSound->Shutdown();
	}
}

void SoundPlayer::detail_SetPlayableSoundLimit(int limit)
{

	mPlayableLimit = limit;
}

bool SoundPlayer::detail_CanPlaySound(int startPriority)
{
	detail::SoundThread::AutoLock lock;

	if (GetPlayableSoundCount() == 0)
		return false;

	if (GetPlayingSoundCount() >= GetPlayableSoundCount())
	{
		detail::BasicSound *dropSound = GetLowestPrioritySound();
		if (!dropSound)
			return false;

		if (startPriority < dropSound->CalcCurrentPlayerPriority())
			return false;
	}

	return true;
}

void SoundPlayer::detail_AppendPlayerHeap(detail::PlayerHeap *heap)
{

	detail::SoundThread::AutoLock lock;

	heap->AttachSoundPlayer(this);
	mHeapList.PushBack(heap);
}

detail::PlayerHeap *SoundPlayer::detail_AllocPlayerHeap(detail::BasicSound *sound)
{

	detail::SoundThread::AutoLock lock;

	if (mHeapList.IsEmpty())
		return nullptr;

	detail::PlayerHeap &playerHeap = mHeapList.GetFront();
	mHeapList.PopFront();

	playerHeap.AttachSound(sound);
	sound->AttachPlayerHeap(&playerHeap);
	playerHeap.Clear();

	return &playerHeap;
}

void SoundPlayer::detail_FreePlayerHeap(detail::BasicSound *sound)
{

	detail::SoundThread::AutoLock lock;

	detail::PlayerHeap *heap = sound->GetPlayerHeap();
	if (!heap)
		return;

	heap->DetachSound(sound);
	sound->DetachPlayerHeap(heap);
	mHeapList.PushBack(heap);
}

}}
