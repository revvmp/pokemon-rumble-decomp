#include <revolution/gx.h>

GXRenderModeObj GXNtsc480IntDf = {
    VI_TVMODE_NTSC_INT,
    640, 480, 480, 40, 0, 640, 480,
    VI_XFBMODE_DF,
    0, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8}
};

GXRenderModeObj GXNtsc480Prog = {
    VI_TVMODE_NTSC_PROG,
    640, 480, 480, 40, 0, 640, 480,
    VI_XFBMODE_SF,
    0, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0}
};

GXRenderModeObj GXMpal480IntDf = {
    VI_TVMODE_MPAL_INT,
    640, 480, 480, 40, 0, 640, 480,
    VI_XFBMODE_DF,
    0, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8}
};

GXRenderModeObj GXPal528IntDf = {
    VI_TVMODE_PAL_INT,
    640, 528, 528, 40, 23, 640, 528,
    VI_XFBMODE_DF,
    0, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8}
};

GXRenderModeObj GXEurgb60Hz480IntDf = {
    VI_TVMODE_EURGB60_INT,
    640, 480, 480, 40, 0, 640, 480,
    VI_XFBMODE_DF,
    0, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8}
};

GXRenderModeObj GXEurgb60Hz480Prog = {
    VI_TVMODE_EURGB60_PROG,
    640, 480, 480, 40, 0, 640, 480,
    VI_XFBMODE_SF,
    0, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0}
};
