#ifndef GAME_CCONTROLLERCOMPOSITION_H
#define GAME_CCONTROLLERCOMPOSITION_H

#include <game/BController.h>
#include <vector>

class CControllerComposition : public BController {
public:
    virtual Type GetType() const;
    virtual void Update();
    void SetControllers(const std::vector<IController*>& rControllers);
    virtual void StartMotor(const MotorPattern& rPattern) const;
    virtual void StopMotor() const;
    virtual const wchar_t* GetButtonSymbol(u32 button) const;

    const std::vector<IController*>& GetControllers() const { return mControllers; }

private:
    std::vector<IController*> mControllers;
};

#endif
