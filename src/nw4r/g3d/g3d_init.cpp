#include <nw4r/g3d/g3d_init.h>
#include <nw4r/g3d/g3d_state.h>
#include <nw4r/g3d/platform/g3d_cpu.h>
#include <nw4r/ut/ut_LockedCache.h>

#include <revolution/GX.h>
#include <revolution/OS.h>
#include <revolution/VI.h>

namespace {

NW4R_LIB_VERSION(G3D, "Sep 24 2009", "16:53:04", "0x4302_151");

}

namespace nw4r {
namespace g3d {

void G3dInit(bool enableLockedCache) {
    OSRegisterVersion(NW4R_G3D_Version_);

    if (enableLockedCache) {
        ut::LC::Enable();
    } else {
        ut::LC::Disable();
    }

    InitFastCast();

    GXRenderModeObj *pMode;
    switch ((u32)VIGetTvFormat()) {
        case VI_NTSC: {
            pMode = &GXNtsc480IntDf;
            break;
        }

        case VI_PAL: {
            pMode = &GXPal528IntDf;
            break;
        }

        case VI_EURGB60: {
            pMode = &GXEurgb60Hz480IntDf;
            break;
        }

        case VI_MPAL: {
            pMode = &GXMpal480IntDf;
            break;
        }

        default: {
            pMode = &GXNtsc480IntDf;
            break;
        }
    }

    G3DState::SetRenderModeObj(*pMode);
}

void G3dReset() {
    G3DState::Invalidate();
}

}
}
