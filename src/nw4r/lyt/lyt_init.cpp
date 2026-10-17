#include <nw4r/lyt/lyt_init.h>

#include <revolution/os.h>
#include <revolution/os/OSFastCast.h>

namespace {

const char* NW4R_LYT_Version_ = "<< NW4R    - LYT \tfinal   build: Sep 24 2009 16:54:15 (0x4302_151) >>";

}

namespace nw4r {
namespace lyt {

void LytInit() {
    OSRegisterVersion(NW4R_LYT_Version_);
    OSInitFastCast();
}

}
}
