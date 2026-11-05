#ifndef GAME_INUMERICINPUT_H
#define GAME_INUMERICINPUT_H

#include <game/core/ICalcDraw.h>
#include <memory>
#include <string>

class INumericInput : public ICalcDraw {
public:
    virtual ~INumericInput() {}
    virtual void Start(int player) = 0;
    virtual bool IsFinished() const = 0;
    virtual std::wstring GetText() const = 0;
    virtual bool IsConfirmed() const = 0;

    static std::tr1::shared_ptr<INumericInput> Create(bool flag);
};

#endif
