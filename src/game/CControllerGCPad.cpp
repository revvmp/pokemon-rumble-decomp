#include <game/CControllerGCPad.h>
#include <game/MotorPattern.h>

inline bool IsGCPadReadable(const GCPadState* pPad) {
    return pPad->err == 0 || pPad->err == -3;
}

inline bool IsGCPadConnected(const GCPadState* pPad) {
    return pPad && IsGCPadReadable(pPad);
}

IController::Type CControllerGCPad::GetType() const {
    return TYPE_GC_PAD;
}

void CControllerGCPad::Update() {
    GCPadState* pPad = GetGCPad(GetChannel());
    if (IsGCPadConnected(pPad)) {
        u32 hold = pPad->mHold;
        u32 buttons = 0;
        if (hold & 0x10008) buttons |= 1;
        if (hold & 0x20004) buttons |= 4;
        if (hold & 0x40001) buttons |= 8;
        if (hold & 0x80002) buttons |= 2;
        if (hold & 0x200) buttons |= 16;
        if (hold & 0x100) buttons |= 32;
        if (hold & 0xC00) buttons |= 128;
        if (hold & 0x10) buttons |= 2048;
        if (hold & 0x1000) buttons |= 256;
        SetButton(buttons);
        SetStick(*reinterpret_cast<const nw4r::math::VEC2*>(&pPad->mStick));
        if (GetStickLength() == 0.0f) {
            SetStickFromCross();
        }
    } else {
        SetDisconnected();
    }
}

void CControllerGCPad::StartMotor(const MotorPattern& rPattern) {
    StartGCRumble(GetChannel(), rPattern.mpGCPattern);
}

void CControllerGCPad::StopMotor() {
    StopGCRumble(GetChannel());
}

const wchar_t* CControllerGCPad::GetButtonSymbol(u32 button) const {
    switch (button) {
    case 1:
    case 2:
    case 4:
    case 8:
        return L"\x1\x2\xE108\x1\x1";
    case 32:
        return L"\x1\x2\xE103\x1\x1";
    case 16:
        return L"\x1\x2\xE104\x1\x1";
    case 128:
        return L"\x1\x2\xE105\x1\x1";
    case 256:
        return L"\x1\x2\xE107\x1\x1";
    case 1024:
        return L"";
    case 2048:
        return L"\x1\x2\xE10C\x1\x1";
    default:
        return L"";
    }
}
