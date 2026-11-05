#include <game/graphics/CPostEffect.h>

const CPostEffect::EffectInfo gPostEffectInfos[8] = {
    {0, "contrast.brres", "contrast__begin", 8.0f, 6.0f, 0, 2},
    {1, "contrast.brres", "contrast__loop", 8.0f, 6.0f, 0, 2},
    {2, "contrast.brres", "contrast__end", 8.0f, 6.0f, 0, 2},
    {3, "vibrate.brres", "vibrate_gate_open", 8.0f, 6.0f, 0, 2},
    {4, "vibrate.brres", "vibrate_gate_close", 8.0f, 6.0f, 0, 2},
    {5, "vibrate.brres", "vibrate_horizontal", 8.0f, 6.0f, 0, 2},
    {6, "vibrate.brres", "vibrate_vertical", 8.0f, 6.0f, 0, 2},
    {7, "zoom_slow.brres", nullptr, 8.0f, 6.0f, 0, 2}
};
