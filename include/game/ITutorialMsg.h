#ifndef GAME_ITUTORIALMSG_H
#define GAME_ITUTORIALMSG_H

#include <game/ICalcDraw.h>
#include <memory>

class ITutorialMsg : public ICalcDraw {
public:
    virtual ~ITutorialMsg() {}

    static std::tr1::shared_ptr<ITutorialMsg> Create(bool flag);
};

#endif
