#include <game/CControllerClassic.h>

bool IsExtensionControllerEnabled();

IController::Type CControllerClassic::GetType() const {
    return TYPE_CLASSIC;
}

void CControllerClassic::Update() {
    if (!IsExtensionControllerEnabled()) {
        SetDisconnected();
        return;
    }
    unsigned int device;
    int result = GetWiiPadDevice(GetChannel(), &device);
    bool detected = false;
    if ((result == 0 && device == 2) || (result == -2 && IsConnected())) {
        detected = true;
    }
    bool lost = IsLost(detected);
    WiiPadState* pPad = GetWiiPad(GetChannel());
    if (!lost) {
        SetButton(GetClassicButtons(pPad));
        if (pPad->wpad_err == 0 && pPad->dev_type == 2) {
            SetStick(*reinterpret_cast<const nw4r::math::VEC2*>(&pPad->ex_status.cl.lstick));
        }
        if (GetStickLength() == 0.0f) {
            SetStickFromCross();
        }
    } else {
        SetDisconnected();
    }
}

const wchar_t* CControllerClassic::GetButtonSymbol(u32 button) const {
    switch (button) {
    case 1:
    case 2:
    case 4:
    case 8:
        return L"\xE041";
    case 32:
        return L"\xE04C";
    case 16:
        return L"\xE04D";
    case 128:
        return L"\xE04E";
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

const wchar_t* GetClassicButtonSymbol(u32 button) {
    switch (button) {
    case 1:
    case 2:
    case 4:
    case 8:
        return L"\xE041";
    case 32:
        return L"\xE04C";
    case 16:
        return L"\xE04D";
    case 128:
        return L"\xE04E";
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

u32 GetClassicButtons(const WiiPadState* pPad) {
    u32 hold;
    u32 buttons = 0;
    if (pPad->wpad_err == 0 && pPad->dev_type == 2) {
        hold = pPad->ex_status.cl.hold;
        if ((hold & 1) || 0.5f < pPad->ex_status.cl.lstick.y) buttons |= 1;
        if ((hold & 0x4000) || pPad->ex_status.cl.lstick.y < -0.5f) buttons |= 4;
        if ((hold & 2) || pPad->ex_status.cl.lstick.x < -0.5f) buttons |= 8;
        if ((hold & 0x8000) || 0.5f < pPad->ex_status.cl.lstick.x) buttons |= 2;
        if (hold & 0x40) buttons |= 16;
        if (hold & 0x10) buttons |= 32;
        if (hold & 0x28) buttons |= 128;
        if (hold & 0x400) buttons |= 256;
        if (hold & 0x1000) buttons |= 2048;
        if (hold & 0x800) buttons |= 1024;
    }
    return buttons;
}
