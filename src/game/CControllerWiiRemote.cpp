#include <game/CControllerWiiRemote.h>
#include <game/CControllerClassic.h>
#include <game/MotorPattern.h>
#include <game/CControllerComposition.h>
#include <game/ControllerTable.h>
#include <algorithm>

bool IsExtensionControllerEnabled();

inline bool IsWiiPadReadable(const WiiPadState* pPad) {
    return pPad->wpad_err == 0 || pPad->wpad_err == -7;
}

inline bool IsWiiPadConnected(const WiiPadState* pPad) {
    return pPad && IsWiiPadReadable(pPad);
}

inline bool IsWiiPadActive(const WiiPadState* pPad) {
    return pPad && pPad->mProcessingState == 0;
}

inline bool CanReadWiiPad(const WiiPadState* pPad) {
    return IsWiiPadActive(pPad) && IsWiiPadConnected(pPad);
}

CControllerWiiRemote::CControllerWiiRemote() : mUsesClassic(false) {}

IController::Type CControllerWiiRemote::GetType() const {
    return TYPE_WII_REMOTE;
}

void CControllerWiiRemote::Update() {
    unsigned int device;
    int result = GetWiiPadDevice(GetChannel(), &device);
    bool detected = false;
    if (result == 0 || (result == -2 && IsConnected())) {
        detected = true;
    }
    bool lost = IsLost(detected);
    WiiPadState* pPad = GetWiiPad(GetChannel());
    if (!IsExtensionControllerEnabled() && !lost && pPad->wpad_err == 0 && pPad->dev_type == 2) {
        SetButton(GetClassicButtons(pPad));
        if (pPad->wpad_err == 0 && pPad->dev_type == 2) {
            SetStick(*reinterpret_cast<const nw4r::math::VEC2*>(&pPad->ex_status.cl.lstick));
        }
        if (GetStickLength() == 0.0f) {
            SetStickFromCross();
        }
        mUsesClassic = true;
    } else {
        mUsesClassic = false;
        if (!lost) {
            u32 buttons = 0;
            if (CanReadWiiPad(pPad)) {
                u32 hold = pPad->hold;
                if (hold & 2) buttons |= 1;
                if (hold & 1) buttons |= 4;
                if (hold & 8) buttons |= 8;
                if (hold & 4) buttons |= 2;
                if (hold & 0x200) buttons |= 16;
                if (hold & 0x100) buttons |= 32;
                if (hold & 0x800) buttons |= 128;
                if (hold & 0x10) buttons |= 256;
                if (hold & 0x1000) buttons |= 2048;
                if (hold & 0x8000) buttons |= 1024;
            } else {
                buttons = GetHold();
            }
            SetButton(buttons);
            SetStickFromCross();
        } else {
            SetDisconnected();
        }
    }
}

void CControllerWiiRemote::StartMotor(const MotorPattern& rPattern) {
    if (!mUsesClassic) {
        StartWiiRumble(GetChannel(), rPattern.mpWiiPattern);
    }
}

void CControllerWiiRemote::StopMotor() {
    StopWiiRumble(GetChannel());
}

const wchar_t* CControllerWiiRemote::GetButtonSymbol(u32 button) const {
    if (mUsesClassic) {
        return GetClassicButtonSymbol(button);
    }
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

int GetControllerDisconnectState() {
    std::vector<IController*>::const_iterator p;
    int state = 0;
    CControllerComposition* pTable = GetControllerTable();
    p = pTable->GetControllers().begin();
    for (; p != pTable->GetControllers().end(); ++p) {
        IController* pController = *p;
        if (pController) {
            int controllerState = 0;
            if (pController->IsConnected()) {
                if (pController->GetType() == IController::TYPE_WII_REMOTE && !IsExtensionControllerEnabled()) {
                    unsigned int device;
                    if (GetWiiPadDevice(pController->GetChannel(), &device) == 0 && device != 0 && device != 2) {
                        controllerState = 5;
                    }
                }
            } else {
                IController::Type type = pController->GetType();
                switch (type) {
                case IController::TYPE_GC_PAD:
                    controllerState = 1;
                    break;
                case IController::TYPE_NUNCHAKU:
                    if (GetController(IController::TYPE_WII_REMOTE, pController->GetChannel())->IsConnected()) {
                        controllerState = 2;
                    } else {
                        controllerState = 4;
                    }
                    break;
                case IController::TYPE_CLASSIC:
                    if (GetController(IController::TYPE_WII_REMOTE, pController->GetChannel())->IsConnected()) {
                        controllerState = 3;
                    } else {
                        controllerState = 4;
                    }
                    break;
                case IController::TYPE_WII_REMOTE:
                    controllerState = 4;
                    break;
                }
            }
            state = std::max(int(state), int(controllerState));
        }
    }
    return state;
}
