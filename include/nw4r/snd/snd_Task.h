#ifndef NW4R_SND_TASK_H
#define NW4R_SND_TASK_H
#include <nw4r/ut/ut_NonCopyable.h>

#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/ut/ut_LinkList.h>

namespace nw4r { namespace snd { namespace detail
{

    class Task : private ut::NonCopyable
    {

    public:
        typedef ut::LinkList<Task, 0x04> LinkList;

    public:
        Task() : mBusyFlag(false) {}
        virtual ~Task();

        virtual void Execute() = 0;
        virtual void Cancel() = 0;
        virtual void OnCancel() = 0;

    private:
        ut::LinkListNode mTaskLink;
        bool mBusyFlag;

    private:
        friend class TaskManager;
    };
}}}

#endif
