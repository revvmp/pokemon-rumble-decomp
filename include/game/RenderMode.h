#ifndef GAME_RENDER_MODE_H
#define GAME_RENDER_MODE_H

#include <revolution/gx/GXStruct.h>

GXRenderModeObj ChooseDefaultRenderMode();
f32 GetDisplayAspectRatio();
f32 GetPixelAspectRatio();
f32 GetPixelAspectRatio(u32 tvMode, u32 aspect);
void GetSafeDisplaySize(u16* pWidth, u16* pHeight, u32 tvMode, u32 aspect);

#endif
