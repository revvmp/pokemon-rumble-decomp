#ifndef GAME_CCONTROLLERWIIREMOTE_H
#define GAME_CCONTROLLERWIIREMOTE_H

#include <game/BController.h>

class CControllerWiiRemote : public BController {
public:
    CControllerWiiRemote();
    virtual Type GetType() const;
    virtual void Update();
    virtual void StartMotor(const MotorPattern& rPattern) const;
    virtual void StopMotor() const;
    virtual const wchar_t* GetButtonSymbol(u32 button) const;
    bool UsesClassic() const { return mUsesClassic; }

private:
    bool mUsesClassic;
};

#endif
