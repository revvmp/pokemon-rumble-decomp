#include "nw4r/snd/snd_TaskThread.h"






#include "nw4r/snd/snd_TaskManager.h"

#include <revolution/os/OSThread.h>




namespace nw4r { namespace snd { namespace detail {

TaskThread::TaskThread() :
	mStackEnd	(nullptr),
	mFinishFlag	(false),
	mCreateFlag	(false)
{
}

TaskThread::~TaskThread()
{
	if (mCreateFlag)
		Destroy();
}

bool TaskThread::Create(s32 priority, void *stack, u32 stackSize)
{



	if (mCreateFlag)
		Destroy();

	BOOL result = OSCreateThread(&mThread, &ThreadFunc, this,
	                             static_cast<u8 *>(stack) + stackSize,
	                             stackSize, priority, 0);
	if (!result)
		return false;

	mStackEnd	= static_cast<u32 *>(stack);
	mFinishFlag	= false;
	mCreateFlag	= true;

	OSResumeThread(&mThread);

	return true;
}

void TaskThread::Destroy()
{
	if (!mCreateFlag)
		return;

	mFinishFlag = true;
	TaskManager::GetInstance().CancelWaitTask();

	BOOL result = OSJoinThread(&mThread, nullptr);


	mCreateFlag = false;
}

void *TaskThread::ThreadFunc(void *arg)
{
	TaskThread *taskThread = static_cast<TaskThread *>(arg);

	taskThread->ThreadProc();

	return nullptr;
}

void TaskThread::ThreadProc()
{
	while (!mFinishFlag)
	{
		TaskManager::GetInstance().WaitTask();

		if (mFinishFlag)
			break;

		TaskManager::GetInstance().ExecuteTask();


	}
}

}}}
