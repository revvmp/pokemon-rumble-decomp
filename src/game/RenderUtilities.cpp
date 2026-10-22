#include <game/MathUtil.h>
#include <game/Memory.h>
#include <game/RenderMode.h>

#include <math.h>
#include <memory>
#include <nw4r/g3d/g3d_state.h>
#include <revolution/cx.h>
#include <revolution/gx.h>
#include <revolution/os/OSCache.h>
#include <revolution/vi.h>

extern "C" u8 SCGetAspectRatio();
extern "C" u8 SCGetEuRgb60Mode();

void GetDisplaySize(u16* pWidth, u16* pHeight, u32 tvMode);

f32 GetPixelAspectRatio() {
    const u8 aspect = SCGetAspectRatio();
    const u32 tvFormat = VIGetTvFormat();
    switch (tvFormat) {
    default:
        return aspect == 1 ? 1.2f : 0.9f;
    case VI_PAL:
    case VI_DEBUG_PAL:
        return aspect == 1 ? 1.45f : 1.09f;
    }
}

f32 GetPixelAspectRatio(u32 tvMode, u32 aspect) {
    switch (tvMode >> 2) {
    default:
        return aspect == 1 ? 1.2f : 0.9f;
    case VI_PAL:
    case VI_DEBUG_PAL:
        return aspect == 1 ? 1.45f : 1.09f;
    }
}

f32 GetDisplayAspectRatio() {
    const u8 aspect = SCGetAspectRatio();
    const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
    const f32 ratio = GetPixelAspectRatio(pMode->viTVmode, aspect);
    return pMode->viWidth * ratio / pMode->viHeight;
}

GXRenderModeObj ChooseDefaultRenderMode() {
    GXRenderModeObj mode;
    if (VIGetScanMode() == VI_PROGRESSIVE) {
        const u32 tvFormat = VIGetTvFormat();
        if (tvFormat == VI_PAL || tvFormat == VI_EURGB60) {
            mode = GXEurgb60Hz480Prog;
        } else {
            mode = GXNtsc480Prog;
        }
    } else {
        const u32 tvFormat = VIGetTvFormat();
        if (tvFormat == VI_PAL || tvFormat == VI_EURGB60) {
            if (SCGetEuRgb60Mode() == 1) {
                mode = GXEurgb60Hz480IntDf;
            } else {
                mode = GXPal528IntDf;
            }
        } else {
            mode = GXNtsc480IntDf;
        }
    }
    u16 width;
    GetDisplaySize(&width, nullptr, mode.viTVmode);
    if (SCGetAspectRatio() == 1) {
        mode.viWidth = width;
    } else {
        mode.viWidth = width - 50;
    }
    mode.viXOrigin = (width - mode.viWidth) / 2;
    return mode;
}

void GetDisplaySize(u16* pWidth, u16* pHeight, u32 tvMode) {
    u16 ignored;
    if (pWidth == nullptr) {
        pWidth = &ignored;
    }
    if (pHeight == nullptr) {
        pHeight = &ignored;
    }
    switch (tvMode) {
    default:
        *pWidth = VI_MAX_WIDTH_NTSC;
        *pHeight = VI_MAX_HEIGHT_NTSC;
        break;
    case VI_TVMODE_PAL_INT:
    case VI_TVMODE_PAL_DS:
        *pWidth = VI_MAX_WIDTH_PAL;
        *pHeight = VI_MAX_HEIGHT_PAL;
        break;
    case VI_TVMODE_EURGB60_INT:
    case VI_TVMODE_EURGB60_DS:
    case VI_TVMODE_EURGB60_PROG:
        *pWidth = VI_MAX_WIDTH_EURGB60;
        *pHeight = VI_MAX_HEIGHT_EURGB60;
        break;
    case VI_TVMODE_MPAL_INT:
    case VI_TVMODE_MPAL_DS:
    case VI_TVMODE_MPAL_PROG:
        *pWidth = VI_MAX_WIDTH_MPAL;
        *pHeight = VI_MAX_HEIGHT_MPAL;
        break;
    }
}

void GetSafeDisplaySize(u16* pWidth, u16* pHeight, u32 tvMode, u32 aspect) {
    u16 ignored;
    if (pWidth == nullptr) {
        pWidth = &ignored;
    }
    if (pHeight == nullptr) {
        pHeight = &ignored;
    }
    switch (tvMode) {
    default:
        if (aspect == 1) {
            *pWidth = 626;
            *pHeight = 418;
        } else {
            *pWidth = 608;
            *pHeight = 388;
        }
        break;
    case VI_TVMODE_PAL_INT:
    case VI_TVMODE_PAL_DS:
        if (aspect == 1) {
            *pWidth = 626;
            *pHeight = 500;
        } else {
            *pWidth = 608;
            *pHeight = 464;
        }
        break;
    }
}

std::tr1::shared_ptr<u8> DecompressLZ(const void* pCompressed) {
    const u32 size = CXGetUncompressedSize(pCompressed);
    std::tr1::shared_ptr<u8> result(static_cast<u8*>(GameMemory::AllocateMEM2((size + 31) & ~31, 32)), GameMemory::Free);
    CXUncompressLZ(pCompressed, result.get());
    DCFlushRange(result.get(), size);
    return result;
}

std::tr1::shared_ptr<u8> DecompressHuffman(const void* pCompressed) {
    const u32 size = CXGetUncompressedSize(pCompressed);
    std::tr1::shared_ptr<u8> result(static_cast<u8*>(GameMemory::AllocateMEM2((size + 31) & ~31, 32)), GameMemory::Free);
    CXUncompressHuffman(pCompressed, result.get());
    DCFlushRange(result.get(), size);
    return result;
}

f32 PositiveIntegerPower(f32 value, int exponent) {
    if (exponent == 1) {
        return value;
    }
    const f32 half = PositiveIntegerPower(value, exponent / 2);
    if (exponent % 2 == 0) {
        return half * half;
    }
    return value * (half * half);
}

f32 IntegerPower(f32 value, int exponent) {
    if (exponent == 0) {
        return 1.0f;
    }
    f32 zero = 0.0f;
    if (exponent < 0) {
        return PositiveIntegerPower(1.0f / value, -exponent);
    }
    return PositiveIntegerPower(value, exponent);
}

f32 NormalizeSignedAngle(f32 angle) {
    f32 adjusted;
    const f32 value = fmod(angle, 360.0);
    if (value < -180.0f) {
        adjusted = value;
        adjusted += 360.0f;
        return 180.0f <= adjusted ? -180.0f : adjusted;
    } else if (180.0f <= value) {
        adjusted = value - 360.0f;
        return adjusted < -180.0f ? -180.0f : adjusted;
    }
    return value;
}

f32 NormalizeAngle(f32 angle) {
    angle = fmod(angle, 360.0);
    if (angle < 0.0f) {
        angle += 360.0f;
        const bool outOfRange = angle <= 0.0f || 360.0f <= angle;
        if (outOfRange) {
            return 0.0f;
        }
    }
    return angle;
}
