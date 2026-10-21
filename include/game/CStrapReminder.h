#ifndef GAME_CSTRAPREMINDER_H
#define GAME_CSTRAPREMINDER_H

#include <game/CLayout.h>
#include <game/CFiber.h>
#include <game/IStrapReminder.h>
#include <memory>

class CStrapReminder : public IStrapReminder {
public:
    CStrapReminder() : mCloseQueued(false) {}
    virtual ~CStrapReminder() {}
    virtual void Finish();
    virtual void Calc();
    virtual void Draw();
    static std::tr1::shared_ptr<IStrapReminder> Create(bool background);

private:
    void Initialize(bool background);
    void Run();

    CLayout mLayout;
    CFiber mFiber;
    bool mCloseQueued;
};

#endif
