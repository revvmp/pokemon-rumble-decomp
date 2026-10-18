#ifndef GAME_GRAPHICS_H
#define GAME_GRAPHICS_H

#include <revolution/gx/GXStruct.h>

namespace Graphics {

void Initialize(const GXRenderModeObj* pModeInput);
void CopyFrameBuffer();
void DisplayFrameBuffer();
void ResetState();
void SetPixelFormat(GXPixelFmt pixelFormat, GXZFmt16 depthFormat);
void SetCopyClear(const GXColor& color, u32 depth, GXPixelFmt pixelFormat);

}

#endif
