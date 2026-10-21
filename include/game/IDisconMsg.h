#ifndef GAME_IDISCONMSG_H
#define GAME_IDISCONMSG_H

#include <game/ICalcDraw.h>
#include <memory>

class IDisconMsg : public ICalcDraw {
public:
    class CDisable {
    public:
        explicit CDisable(IDisconMsg* pMsg) : mpMsg(pMsg) { pMsg->AdjustSuppressCount(1); }
        ~CDisable() { mpMsg->AdjustSuppressCount(-1); }

    private:
        IDisconMsg* mpMsg;
    };

    virtual bool IsActive() const = 0;
    virtual bool CanShow() const = 0;
    virtual void AdjustSuppressCount(int amount) = 0;
};
std::tr1::shared_ptr<IDisconMsg> CreateDisconMsg(bool background);

#endif
