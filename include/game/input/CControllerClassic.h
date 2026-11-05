#ifndef GAME_CCONTROLLERCLASSIC_H
#define GAME_CCONTROLLERCLASSIC_H

#include <game/input/BController.h>
#include <game/input/Pad.h>

class CControllerClassic : public BController {
public:
    virtual Type GetType() const;
    virtual void Update();
    virtual const wchar_t* GetButtonSymbol(u32 button) const;
};

const wchar_t* GetClassicButtonSymbol(u32 button);
u32 GetClassicButtons(const WiiPadState* pPad);

#endif
