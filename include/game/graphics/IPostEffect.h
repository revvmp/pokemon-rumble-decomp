#ifndef GAME_IPOSTEFFECT_H
#define GAME_IPOSTEFFECT_H

#include <game/core/ICalcDraw.h>
#include <memory>

class IPostEffect : public ICalcDraw {
public:
    virtual ~IPostEffect() {}
    virtual void Show(int effect) = 0;
    virtual void Hide() = 0;

    static std::tr1::shared_ptr<IPostEffect> Create(bool a, bool b);
};

#endif
