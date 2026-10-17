#include "nw4r/snd/snd_VoiceManager.h"

#include <new>

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_DisposeCallbackManager.h"
#include "nw4r/snd/snd_Voice.h"

#include "nw4r/ut/ut_Lock.h"

namespace nw4r { namespace snd { namespace detail {

VoiceManager &VoiceManager::GetInstance()
{
	static VoiceManager instance;

	return instance;
}

VoiceManager::VoiceManager() :
	mInitialized(false)
{
}

u32 VoiceManager::GetRequiredMemSize(int voiceCount)
{
	return sizeof(Voice) * voiceCount;
}

void VoiceManager::Setup(void *mem, u32 memSize)
{
	if (mInitialized)
		return;

	u32 voiceCount = memSize / sizeof(Voice);
	u8 *ptr = static_cast<u8 *>(mem);

	for (int i = 0; i < voiceCount; i++)
	{
		mFreeVoiceList.PushBack(new (ptr) Voice);

		ptr += sizeof(Voice);
	}

	mInitialized = true;
}

void VoiceManager::Shutdown()
{
	if (!mInitialized)
		return;

	StopAllVoices();

	while (!mFreeVoiceList.IsEmpty())
	{
		Voice &voice = mFreeVoiceList.GetFront();
		mFreeVoiceList.PopFront();

		voice.~Voice();
	}

	mInitialized = false;
}

void VoiceManager::StopAllVoices()
{
	ut::AutoInterruptLock lock;

	while (!mPrioVoiceList.IsEmpty())
	{
		Voice &voice = mPrioVoiceList.GetFront();

		voice.Stop();

		if (voice.mCallback)
		{
			(*voice.mCallback)(&voice, Voice::CALLBACK_STATUS_CANCEL,
			                   voice.mCallbackArg);
		}

		voice.Free();
	}
}

Voice *VoiceManager::AllocVoice(int voiceChannelCount, int voiceOutCount,
                                int priority, Voice::VoiceCallback callback,
                                void *callbackData)
{
	ut::AutoInterruptLock lock;

	if (mFreeVoiceList.IsEmpty() && !DropLowestPriorityVoice(priority))
		return nullptr;

	Voice &voice = mFreeVoiceList.GetFront();
	if (!voice.Acquire(voiceChannelCount, voiceOutCount, priority, callback,
	                   callbackData))
	{
		
		return nullptr;
	}

	voice.mPriority = priority & Voice::PRIORITY_MAX;
	AppendVoiceList(&voice);
	UpdateEachVoicePriority(mPrioVoiceList.GetIteratorFromPointer(&voice),
	                        mPrioVoiceList.GetEndIter());
	DisposeCallbackManager::GetInstance().RegisterDisposeCallback(&voice);

	return &voice;
}

void VoiceManager::FreeVoice(Voice *voice)
{

	ut::AutoInterruptLock lock;

	DisposeCallbackManager::GetInstance().UnregisterDisposeCallback(voice);
	RemoveVoiceList(voice);
}

void VoiceManager::UpdateAllVoices()
{
	for (VoiceList::Iterator itr = mPrioVoiceList.GetBeginIter(); itr != mPrioVoiceList.GetEndIter();)
	{
		VoiceList::Iterator curItr = itr++;

		curItr->StopFinished();
	}

	for (VoiceList::Iterator itr = mPrioVoiceList.GetBeginIter(); itr != mPrioVoiceList.GetEndIter();)
	{
		VoiceList::Iterator curItr = itr++;

		curItr->Calc();
	}

	ut::AutoInterruptLock lock;

	for (VoiceList::Iterator itr = mPrioVoiceList.GetBeginIter(); itr != mPrioVoiceList.GetEndIter();)
	{
		VoiceList::Iterator curItr = itr++;

		curItr->Update();
	}
}

void VoiceManager::NotifyVoiceUpdate()
{
	ut::AutoInterruptLock lock;

	for (VoiceList::Iterator itr = mPrioVoiceList.GetBeginIter(); itr != mPrioVoiceList.GetEndIter();)
	{
		VoiceList::Iterator curItr = itr++;

		curItr->ResetDelta();
	}
}

void VoiceManager::AppendVoiceList(Voice *voice)
{
	ut::AutoInterruptLock lock;

	mFreeVoiceList.Erase(voice);

	VoiceList::RevIterator it = mPrioVoiceList.GetEndReverseIter();
	for (; it != mPrioVoiceList.GetBeginReverseIter(); ++it)
	{
		if (it->GetPriority() <= voice->GetPriority())
			break;
	}

	mPrioVoiceList.Insert(it.GetBase(), voice);
}

void VoiceManager::RemoveVoiceList(Voice *voice)
{
	ut::AutoInterruptLock lock;

	mPrioVoiceList.Erase(voice);
	mFreeVoiceList.PushBack(voice);
}

void VoiceManager::ChangeVoicePriority(Voice *voice)
{
	ut::AutoInterruptLock lock;

	RemoveVoiceList(voice);
	AppendVoiceList(voice);

	UpdateEachVoicePriority(mPrioVoiceList.GetIteratorFromPointer(voice),
	                        mPrioVoiceList.GetEndIter());
}

void VoiceManager::UpdateEachVoicePriority(
	VoiceList::Iterator const &beginItr,
	VoiceList::Iterator const &endItr)
{
	for (VoiceList::Iterator it = beginItr; it != endItr; ++it)
	{
		if (it->GetPriority() <= 1)
			return;

		if (it->GetPriority() != Voice::PRIORITY_MAX)
			it->UpdateVoicesPriority();
	}
}

void VoiceManager::UpdateAllVoicesSync(u32 syncFlag)
{
	ut::AutoInterruptLock lock;

	for (VoiceList::Iterator itr = mPrioVoiceList.GetBeginIter(); itr != mPrioVoiceList.GetEndIter();)
	{
		VoiceList::Iterator curItr = itr++;

		if (curItr->mIsActive)
			curItr->mSyncFlag |= syncFlag;
	}
}

int VoiceManager::DropLowestPriorityVoice(int priority)
{
	int dropCount = 0;

	if (mFreeVoiceList.IsEmpty())
	{
		Voice &dropVoice = mPrioVoiceList.GetFront();

		if (dropVoice.GetPriority() > priority)
			return 0;

		dropCount = dropVoice.GetAxVoiceCount();

		dropVoice.Stop();
		dropVoice.Free();

		if (dropVoice.mCallback)
		{
			(*dropVoice.mCallback)(&dropVoice,
			                       Voice::CALLBACK_STATUS_DROP_VOICE,
			                       dropVoice.mCallbackArg);
		}
	}

	return dropCount;
}

}}}
