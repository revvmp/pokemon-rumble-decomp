#ifndef GAME_EFFECTGROUPID_H
#define GAME_EFFECTGROUPID_H

#include <revolution/types.h>

static const u32 EFFECT_GROUP_INVALID = 0xFFFFFFFF;

u32 AllocEffectGroupID();
void FreeEffectGroupID(u32 id);

#endif