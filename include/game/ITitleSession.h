#ifndef GAME_ITITLESESSION_H
#define GAME_ITITLESESSION_H

#include <game/ISession.h>
#include <memory>

class ITitleSession : public ISession {
public:
    virtual ~ITitleSession() {}

    static std::tr1::shared_ptr<ITitleSession> Create();
};

#endif
