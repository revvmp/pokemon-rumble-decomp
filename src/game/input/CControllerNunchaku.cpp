#include <game/input/CControllerNunchaku.h>
#include <game/input/ControllerTable.h>
#include <game/input/Pad.h>

IController::Type CControllerNunchaku::GetType() const {
    return TYPE_NUNCHAKU;
}

void CControllerNunchaku::Update() {
    if (!IsExtensionControllerEnabled()) {
        SetDisconnected();
        return;
    }
    unsigned int device;
    int result = GetWiiPadDevice(GetChannel(), &device);
    bool detected = false;
    if ((result == 0 && device == 1) || (result == -2 && IsConnected())) {
        detected = true;
    }
    bool lost = IsLost(detected);
    WiiPadState* pPad = GetWiiPad(GetChannel());
    if (!lost) {
        u32 buttons = 0;
        if (pPad->wpad_err == 0 && pPad->dev_type == 1) {
            if (0.5f < pPad->ex_status.fs.stick.y) buttons |= 1;
            if (pPad->ex_status.fs.stick.y < -0.5f) buttons |= 4;
            if (pPad->ex_status.fs.stick.x < -0.5f) buttons |= 8;
            if (0.5f < pPad->ex_status.fs.stick.x) buttons |= 2;
            u32 hold = pPad->hold;
            if (hold & 0x4000) buttons |= 16;
            if (hold & 0x2000) buttons |= 32;
            if (hold & 0x8000) buttons |= 1024;
        }
        SetButton(buttons);
        if (pPad->wpad_err == 0 && pPad->dev_type == 1) {
            SetStick(*reinterpret_cast<const nw4r::math::VEC2*>(&pPad->ex_status.fs.stick));
        }
    } else {
        SetDisconnected();
    }
}

const wchar_t* CControllerNunchaku::GetButtonSymbol(u32 button) const {
    switch (button) {
    case 1:
    case 2:
    case 4:
    case 8:
        return L"\xE049";
    case 32:
        return L"\xE04B";
    case 16:
        return L"\xE04A";
    case 128:
        return L"\xE04B+\xE04A";
    case 256:
        return L"";
    case 1024:
        return L"";
    case 2048:
        return L"";
    default:
        return L"";
    }
}
