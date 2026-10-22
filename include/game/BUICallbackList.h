#ifndef GAME_BUICALLBACKLIST_H
#define GAME_BUICALLBACKLIST_H

#include <game/BUI.h>

inline BUI::CCallbackListOwner::~CCallbackListOwner() { delete mpList; }

#endif
