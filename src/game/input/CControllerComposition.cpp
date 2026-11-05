#include <game/input/CControllerComposition.h>
#include <nw4r/math/math_triangular.h>

inline nw4r::math::VEC2 ScaleStick(f32 scale, const nw4r::math::VEC2& rStick) {
    return nw4r::math::VEC2(scale * rStick.x, scale * rStick.y);
}

IController::Type CControllerComposition::GetType() const {
    return TYPE_COMPOSITION;
}

void CControllerComposition::Update() {
    const std::vector<IController*>& rControllers = mControllers;
    nw4r::math::VEC2 stick(0.0f, 0.0f);
    nw4r::math::VEC2 combinedStick;
    std::vector<IController*>::const_iterator p = rControllers.begin();
    u32 buttons = 0;
    bool connected = false;
    for (; p != rControllers.end(); ++p) {
        IController* pController = *p;
        if (pController->IsConnected()) {
            connected = true;
            buttons |= pController->GetHold();
            if (0.0f < pController->GetStickLength()) {
                f32 sine;
                f32 cosine;
                nw4r::math::SinCosDeg(&sine, &cosine, pController->GetStickAngle());
                f32 length = pController->GetStickLength();
                stick += ScaleStick(length, nw4r::math::VEC2(cosine, sine));
            }
        }
    }
    if (connected) {
        SetButton(buttons);
        combinedStick.x = stick.x;
        combinedStick.y = stick.y;
        SetStick(combinedStick);
    } else {
        SetDisconnected();
    }
}

void CControllerComposition::SetControllers(const std::vector<IController*>& rControllers) {
    mControllers = rControllers;
}

void CControllerComposition::StartMotor(const MotorPattern& rPattern) const {
    const std::vector<IController*>& rControllers = mControllers;
    std::vector<IController*>::const_iterator p = rControllers.begin();
    for (; p != rControllers.end(); ++p) {
        (*p)->StartMotor(rPattern);
    }
}

void CControllerComposition::StopMotor() const {
    const std::vector<IController*>& rControllers = mControllers;
    std::vector<IController*>::const_iterator p = rControllers.begin();
    for (; p != rControllers.end(); ++p) {
        (*p)->StopMotor();
    }
}

const wchar_t* CControllerComposition::GetButtonSymbol(u32 button) const {
    switch (button) {
    case 1:
    case 2:
    case 4:
    case 8:
        return L"\xE041";
    case 32:
        return L"\xE048";
    case 16:
        return L"\xE047";
    case 128:
        return L"\xE042";
    case 256:
        return L"\xE045";
    case 1024:
        return L"\xE044";
    case 2048:
        return L"\xE046";
    default:
        return L"";
    }
}
