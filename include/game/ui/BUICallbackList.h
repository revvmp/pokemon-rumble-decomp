#ifndef GAME_BUICALLBACKLIST_H
#define GAME_BUICALLBACKLIST_H

#include <game/ui/BUI.h>

inline BUI::CCallbackListOwner::~CCallbackListOwner() { delete mpList; }

#endif
