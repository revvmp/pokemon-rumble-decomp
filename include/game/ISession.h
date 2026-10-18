#ifndef GAME_ISESSION_H
#define GAME_ISESSION_H

#include <game/ICalcDraw.h>
#include <memory>

class ISession : public ICalcDraw, public std::tr1::enable_shared_from_this<ISession> {
public:
    virtual ~ISession() {}
    virtual void Run() = 0;
};

#endif
