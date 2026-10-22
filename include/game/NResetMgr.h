#ifndef GAME_NRESETMGR_H
#define GAME_NRESETMGR_H

#include <revolution/types.h>

namespace NResetMgr {

void Initialize();
void Update();
bool IsEnabled();
void ClearDisableCount();
bool RequestLaunchManualViewer(u32 argument);
bool RequestRestart(u32 argument);
bool RequestReturnToDataManager();
bool RequestReturnToMenu();
bool IsRestartRequested();
bool IsShutdownRequested();
bool CancelRestart();

}

#endif
