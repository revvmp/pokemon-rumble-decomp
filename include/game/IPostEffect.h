#ifndef GAME_IPOSTEFFECT_H
#define GAME_IPOSTEFFECT_H

#include <game/ICalcDraw.h>
#include <memory>

class IPostEffect : public ICalcDraw {
public:
    virtual ~IPostEffect() {}

    static std::tr1::shared_ptr<IPostEffect> Create(bool a, bool b);
};

#endif
