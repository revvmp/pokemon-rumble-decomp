#ifndef GAME_DETAIL_CDISCONMSG_H
#define GAME_DETAIL_CDISCONMSG_H

#include <game/IDisconMsg.h>
#include <game/CLayout.h>
#include <game/CFiber.h>

namespace {
class CDisconMsg : public IDisconMsg {
public:
    CDisconMsg() : mSuppressCount(0) {}
    virtual ~CDisconMsg();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual bool IsActive() const;
    virtual bool CanShow() const;
    virtual void AdjustSuppressCount(int amount);
    void Run();
private:
    std::tr1::shared_ptr<CLayout> mLayout;
    std::tr1::shared_ptr<CFiber> mFiber;
    int mSuppressCount;
};
}

#endif
