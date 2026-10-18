#ifndef GAME_RENDER_PRIMITIVE_H
#define GAME_RENDER_PRIMITIVE_H

#include <nw4r/ut/ut_Color.h>

void SetupVertexColorRendering();
void SetupOverlayRendering();
void SetupScreenProjection();
void DrawScreenQuad(f32 x, f32 y, f32 width, f32 height, f32 z, nw4r::ut::Color color);

#endif
