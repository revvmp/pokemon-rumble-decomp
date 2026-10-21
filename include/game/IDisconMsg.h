#ifndef GAME_IDISCONMSG_H
#define GAME_IDISCONMSG_H

#include <game/ICalcDraw.h>
#include <memory>

class IDisconMsg : public ICalcDraw {
public:
    virtual bool IsActive() const = 0;
    virtual bool CanShow() const = 0;
    virtual void AdjustSuppressCount(int amount) = 0;
};
std::tr1::shared_ptr<IDisconMsg> CreateDisconMsg();

#endif
