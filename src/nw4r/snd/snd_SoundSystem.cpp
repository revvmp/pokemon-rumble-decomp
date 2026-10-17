#include "nw4r/snd/snd_SoundSystem.h"

#include "nw4r/snd/snd_AxVoiceManager.h"
#include "nw4r/snd/snd_AxManager.h"
#include "nw4r/snd/snd_ChannelManager.h"
#include "nw4r/snd/snd_RemoteSpeakerManager.h"
#include "nw4r/snd/snd_Types.h"
#include "nw4r/snd/snd_SeqPlayer.h"
#include "nw4r/snd/snd_SoundThread.h"
#include "nw4r/snd/snd_TaskManager.h"
#include "nw4r/snd/snd_TaskThread.h"
#include "nw4r/snd/snd_VoiceManager.h"

#include <revolution/os.h>
#include <revolution/ax.h>
#include <revolution/sc.h>

namespace nw4r { namespace snd
{
	extern "C" char const *NW4R_SND_Version_ = "<< NW4R    - SND \tfinal   build: Sep 24 2009 16:54:03 (0x4302_151) >>";

	detail::TaskThread SoundSystem::sTaskThread;

	namespace
	{
		bool sInitialized;
	}

	int SoundSystem::sMaxVoices;
}}

namespace nw4r { namespace snd {

u32 SoundSystem::GetRequiredMemSize(SoundSystemParam const &param)
{

	int maxVoices = AXGetMaxVoices();

	return param.soundThreadStackSize + param.dvdThreadStackSize
	     + detail::AxVoiceManager::GetInstance().GetRequiredMemSize(maxVoices)
	     + detail::VoiceManager::GetInstance().GetRequiredMemSize(maxVoices)
	     + detail::ChannelManager::GetInstance().GetRequiredMemSize(maxVoices);
}

void SoundSystem::InitSoundSystem(SoundSystemParam const &param, void *workMem,
                                  u32 workMemSize)
{
	bool result;

	if (sInitialized)
		return;

	sInitialized = true;

	OSRegisterVersion(NW4R_SND_Version_);

	detail::AxManager::GetInstance().Init();

	SCInit();

	u32 initStatus;
	do
		initStatus = SCCheckStatus();
	while (initStatus == SC_STATUS_BUSY);

	u8 soundMode = SCGetSoundMode();
	switch (soundMode)
	{
	case SC_SOUND_MODE_MONO:
		detail::AxManager::GetInstance().SetOutputMode(OUTPUT_MODE_MONO);
		break;

	case SC_SOUND_MODE_STEREO:
		detail::AxManager::GetInstance().SetOutputMode(OUTPUT_MODE_STEREO);
		break;

	case SC_SOUND_MODE_SURROUND:
		detail::AxManager::GetInstance().SetOutputMode(OUTPUT_MODE_DPL2);
		break;

	default:
		detail::AxManager::GetInstance().SetOutputMode(OUTPUT_MODE_STEREO);
		break;
	}

	detail::RemoteSpeakerManager::GetInstance().Setup();

	u8 *ptr = static_cast<u8 *>(workMem);

	void *dvdThreadStack = ptr;
	ptr += param.dvdThreadStackSize;

	void *soundThreadStack = ptr;
	ptr += param.soundThreadStackSize;

	sMaxVoices = AXGetMaxVoices();

	void *axVoiceWork = ptr;
	ptr += detail::AxVoiceManager::GetInstance().GetRequiredMemSize(
		sMaxVoices);

	detail::AxVoiceManager::GetInstance().Setup(
		axVoiceWork,
		detail::AxVoiceManager::GetInstance().GetRequiredMemSize(
			sMaxVoices));

	void *voiceWork = ptr;
	ptr += detail::VoiceManager::GetInstance().GetRequiredMemSize(sMaxVoices);

	detail::VoiceManager::GetInstance().Setup(
		voiceWork,
		detail::VoiceManager::GetInstance().GetRequiredMemSize(sMaxVoices));

	void *channelWork = ptr;
	ptr += detail::ChannelManager::GetInstance().GetRequiredMemSize(
		sMaxVoices);

	detail::ChannelManager::GetInstance().Setup(
		channelWork,
		detail::ChannelManager::GetInstance().GetRequiredMemSize(
			sMaxVoices));

	detail::SeqPlayer::InitSeqPlayer();

	result = sTaskThread.Create(param.dvdThreadPriority, dvdThreadStack,
	                   param.dvdThreadStackSize);

	result = detail::SoundThread::GetInstance().Create(
		param.soundThreadPriority, soundThreadStack,
		param.soundThreadStackSize);

}

void SoundSystem::ShutdownSoundSystem()
{
	if (!sInitialized)
		return;

	detail::SoundThread::GetInstance().Shutdown();

	detail::TaskManager::GetInstance().CancelAllTask();
	sTaskThread.Destroy();
	detail::RemoteSpeakerManager::GetInstance().Shutdown();

	detail::ChannelManager::GetInstance().Shutdown();
	detail::VoiceManager::GetInstance().Shutdown();
	detail::AxVoiceManager::GetInstance().Shutdown();
	detail::AxManager::GetInstance().Shutdown();

	sInitialized = false;
}

bool SoundSystem::IsInitializedSoundSystem()
{
	return sInitialized;
}

void SoundSystem::WaitForResetReady() {
    if (!sInitialized) {
        return;
    }

    u32 start = OSGetTick();

    while (!detail::AxManager::GetInstance().IsResetReady()) {
        if (OSTicksToSeconds(OSGetTick() - start) > 0) {
            break;
        }
    }
}

}}
