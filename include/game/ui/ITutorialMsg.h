#ifndef GAME_ITUTORIALMSG_H
#define GAME_ITUTORIALMSG_H

#include <game/core/ICalcDraw.h>
#include <memory>

class ITutorialMsg : public ICalcDraw {
public:
    virtual void Clear() = 0;
    virtual void Show(const wchar_t* pText) = 0;
    virtual void Close() = 0;

    static std::tr1::shared_ptr<ITutorialMsg> Create(bool flag);
};

#endif
