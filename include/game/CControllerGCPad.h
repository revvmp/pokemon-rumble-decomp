#ifndef GAME_CCONTROLLERGCPAD_H
#define GAME_CCONTROLLERGCPAD_H

#include <game/BController.h>

class CControllerGCPad : public BController {
public:
    virtual Type GetType() const;
    virtual void Update();
    virtual void StartMotor(const MotorPattern& rPattern);
    virtual void StopMotor();
    virtual const wchar_t* GetButtonSymbol(u32 button) const;
};

#endif
