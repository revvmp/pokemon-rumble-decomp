#ifndef GAME_ISESSION_H
#define GAME_ISESSION_H

#include <game/ICalcDraw.h>
#include <memory>

class ISession : public ICalcDraw, public std::tr1::enable_shared_from_this<ISession> {
public:
#if defined(CGAMESESSION_EMISSION_LAYOUT)
    virtual ~ISession();
#else
    virtual ~ISession() {}
#endif
    virtual void Run() = 0;
};

#endif
