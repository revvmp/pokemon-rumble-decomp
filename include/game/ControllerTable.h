#ifndef GAME_CONTROLLERTABLE_H
#define GAME_CONTROLLERTABLE_H

#include <game/IController.h>

IController* GetController(IController::Type type, int channel);

#endif
