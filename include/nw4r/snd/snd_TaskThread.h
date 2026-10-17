#ifndef NW4R_SND_TASK_THREAD_H
#define NW4R_SND_TASK_THREAD_H



#include <nw4r/types_nw4r.h>

#include <revolution/os/OSThread.h>



namespace nw4r { namespace snd { namespace detail
{

	class TaskThread
	{

	public:

		TaskThread();
		~TaskThread();


		bool Create(s32 priority, void *stack, u32 stackSize);
		void Destroy();

	private:

		static void *ThreadFunc(void *arg);
		void ThreadProc();


	private:
		OSThread	mThread;
		u32		*mStackEnd;
		bool		mFinishFlag;
		bool		mCreateFlag;

	};
}}}

#endif
