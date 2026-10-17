#ifndef NW4R_SND_TASK_MANAGER_H
#define NW4R_SND_TASK_MANAGER_H

#include <nw4r/snd/snd_Task.h>

#include <revolution/os/OSThread.h>

namespace nw4r { namespace snd { namespace detail
{

    class TaskManager
    {

    public:
        enum TaskPriority
        {
            PRIORITY_LOW,
            PRIORITY_MIDDLE,
            PRIORITY_HIGH,
        };

    public:
        static TaskManager &GetInstance();

        void AppendTask(Task *task, TaskPriority priority);

        Task *ExecuteTask();
        void CancelTask(Task *task);
        void CancelAllTask();

        void WaitTask();
        void CancelWaitTask();

    private:
        TaskManager();

        Task *GetNextTask();
        Task *GetNextTask(TaskPriority priority, bool doRemove);

        Task *PopTask();

    public:
        static int const PRIORITY_NUM = 3;

    private:
        Task::LinkList mTaskList[PRIORITY_NUM];
        Task * volatile mCurrentTask;
        bool mCancelWaitTaskFlag;

        OSThreadQueue mAppendThreadQueue;
        OSThreadQueue mDoneThreadQueue;
    };
}}}

#endif
