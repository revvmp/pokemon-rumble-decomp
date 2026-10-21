#ifndef GAME_CCONTROLLERNUNCHAKU_H
#define GAME_CCONTROLLERNUNCHAKU_H

#include <game/BController.h>

class CControllerNunchaku : public BController {
public:
    virtual Type GetType() const;
    virtual void Update();
    virtual const wchar_t* GetButtonSymbol(u32 button) const;
};

#endif
