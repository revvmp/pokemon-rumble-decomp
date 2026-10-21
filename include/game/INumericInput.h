#ifndef GAME_INUMERICINPUT_H
#define GAME_INUMERICINPUT_H

#include <game/ICalcDraw.h>
#include <memory>

class INumericInput : public ICalcDraw {
public:
    virtual ~INumericInput() {}

    static std::tr1::shared_ptr<INumericInput> Create(bool flag);
};

#endif
