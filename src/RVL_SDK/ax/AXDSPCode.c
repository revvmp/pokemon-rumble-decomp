#include <revolution/ax.h>

u16 axDspSlave[4096] ATTRIBUTE_ALIGN(32) = {
#include <ax_dsp.inc>
};
