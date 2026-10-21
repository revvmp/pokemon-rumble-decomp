#ifndef GAME_CONTROLLERTABLE_H
#define GAME_CONTROLLERTABLE_H

#include <game/IController.h>

class CControllerComposition;

IController* GetController(IController::Type type, int channel);
CControllerComposition* GetControllerTable();
bool IsExtensionControllerEnabled();
int GetControllerDisconnectState();

#endif
