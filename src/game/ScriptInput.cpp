#include <game/CPpuEngine.h>
#include <game/CPlayerPii.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayer.h>
#include <game/CRemoteMemory.h>
#include <game/IGameSession.h>
#include <game/CStageCamera.h>
#include <game/BWorld.h>
#include <game/BWorldLightSet.h>
#include <game/CNullObj.h>
#include <game/ControllerTable.h>
#include <game/MotorPattern.h>
#include <revolution/wpad.h>

IController* GetUIInput();
extern const RumbleStep gMotorPatternShortWii[];
extern const RumbleStep gMotorPatternShortGC[];
extern const RumbleStep gMotorPatternLongWii[];
extern const RumbleStep gMotorPatternLongGC[];
extern const RumbleStep gMotorPatternPulseWii[];
extern const RumbleStep gMotorPatternPulseGC[];

extern const MotorPattern gMotorPatterns[];

namespace {

IController* GetConnectedController(int player) {
    if (player == 4) {
        IController* pInput = GetUIInput();
        if (pInput && pInput->IsConnected()) {
            return pInput;
        }
    } else if (player >= 0) {
        IController* pInput = IGameSession::GetInstance()->GetPlayer(player)->GetController();
        if (pInput && pInput->IsConnected()) {
            return pInput;
        }
    }
    return nullptr;
}

CStageCamera* GetStageCamera(BWorld* pWorld) {
    BWorldCamera* pCamera = pWorld->GetCamera(0);
    if (pCamera) {
        CStageCamera* pStageCamera = DynamicCast<CStageCamera>(pCamera);
        if (pStageCamera) {
            return pStageCamera;
        }
    }
    return nullptr;
}

bool IsLowBattery(IController* pController) {
    if (pController && (pController->GetType() == IController::TYPE_WII_REMOTE ||
                       pController->GetType() == IController::TYPE_CLASSIC ||
                       pController->GetType() == IController::TYPE_NUNCHAKU)) {
        u32 device;
        if (WPADProbe(pController->GetChannel(), &device) == 0 && device <= 2) {
            WPADInfo info;
            if (WPADGetInfo(pController->GetChannel(), &info) == 0 && info.battery <= 1) {
                return true;
            }
        }
    }
    return false;
}

}

int GetPadDirValue(NPpu::CEngine* pEngine) {
    IController* pController = GetConnectedController(pEngine->PopInt());
    pEngine->PushFloat(pController ? pController->GetStickLength() : 0.0f);
    return 0;
}

int GetPadDirDeg(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    IController* pController = GetConnectedController(pEngine->PopInt());
    CStageCamera* pCamera = GetStageCamera(pEngine->GetWorld());
    if (pCamera) {
        pEngine->PushFloat(pCamera->GetStickWorldAngle(pController));
    } else {
        pEngine->PushFloat(0.0f);
    }
    return 0;
}

int GetPadTrig(NPpu::CEngine* pEngine) {
    int player = pEngine->PopInt();
    u32 result = 0;
    if (player == 4) {
        IController* pController = GetUIInput();
        if (pController && pController->IsConnected()) {
            result = pController->GetTrig();
        }
    } else if (player >= 0) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
        if (pPlayer->IsHoldCollectionEnabled()) {
            result = pPlayer->GetCollectedHold() & (pPlayer->GetLastHold() ^ pPlayer->GetCollectedHold());
        } else {
            IController* pController = pPlayer->GetController();
            bool connected = pController && pController->IsConnected();
            result = connected ? pController->GetTrig() : 0;
        }
    }
    pEngine->PushInt(result);
    return 0;
}

int GetPadHold(NPpu::CEngine* pEngine) {
    int player = pEngine->PopInt();
    u32 result = 0;
    if (player == 4) {
        IController* pController = GetUIInput();
        if (pController && pController->IsConnected()) {
            result = pController->GetHold();
        }
    } else if (player >= 0) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
        if (pPlayer->IsHoldCollectionEnabled()) {
            result = pPlayer->GetCollectedHold();
        } else {
            IController* pController = pPlayer->GetController();
            bool connected = pController && pController->IsConnected();
            result = connected ? pController->GetHold() : 0;
        }
    }
    pEngine->PushInt(result);
    return 0;
}

int GetPadRelease(NPpu::CEngine* pEngine) {
    int player = pEngine->PopInt();
    u32 result = 0;
    if (player == 4) {
        IController* pController = GetUIInput();
        if (pController && pController->IsConnected()) {
            result = pController->GetRelease();
        }
    } else if (player >= 0) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
        if (pPlayer->IsHoldCollectionEnabled()) {
            result = pPlayer->GetLastHold() & (pPlayer->GetLastHold() ^ pPlayer->GetCollectedHold());
        } else {
            IController* pController = pPlayer->GetController();
            bool connected = pController && pController->IsConnected();
            result = connected ? pController->GetRelease() : 0;
        }
    }
    pEngine->PushInt(result);
    return 0;
}

int IsControllerNunchaku(NPpu::CEngine* pEngine) {
    int player = pEngine->PopInt();
    IController* pController = IGameSession::GetInstance()->GetPlayer(player)->GetController();
    bool result = false;
    if (pController && pController->GetType() == IController::TYPE_NUNCHAKU) {
        result = true;
    }
    pEngine->PushInt(result);
    return 0;
}

int RTGetPadTrig(NPpu::CEngine* pEngine) {
    IController* pController = GetConnectedController(pEngine->PopInt());
    pEngine->PushInt(pController ? pController->GetTrig() : 0);
    return 0;
}

int RTGetPadHold(NPpu::CEngine* pEngine) {
    IController* pController = GetConnectedController(pEngine->PopInt());
    pEngine->PushInt(pController ? pController->GetHold() : 0);
    return 0;
}

int RTGetPadRelease(NPpu::CEngine* pEngine) {
    IController* pController = GetConnectedController(pEngine->PopInt());
    pEngine->PushInt(pController ? pController->GetRelease() : 0);
    return 0;
}

int RTGetPadRepeat(NPpu::CEngine* pEngine) {
    IController* pController = GetConnectedController(pEngine->PopInt());
    pEngine->PushInt(pController ? pController->GetRepeat() : 0);
    return 0;
}

int StartRumble(NPpu::CEngine* pEngine) {
    int pattern = pEngine->PopInt();
    IController* pController = GetConnectedController(pEngine->PopInt());
    if (pController) {
        pController->StartMotor(gMotorPatterns[pattern]);
    }
    pEngine->PushNull();
    return 0;
}

int StopRumble(NPpu::CEngine* pEngine) {
    IController* pController = GetConnectedController(pEngine->PopInt());
    if (pController) {
        pController->StopMotor();
    }
    pEngine->PushNull();
    return 0;
}

int IsPadLowBattery(NPpu::CEngine* pEngine) {
    int player = pEngine->PopInt();
    bool low = false;
    if (player == 4) {
        for (int i = 0; i < 4; ++i) {
            IController* pController = IGameSession::GetInstance()->GetPlayer(i)->GetController();
            u32 device;
            WPADInfo info;
            if (pController && (pController->GetType() == IController::TYPE_WII_REMOTE ||
                                  pController->GetType() == IController::TYPE_CLASSIC ||
                                  pController->GetType() == IController::TYPE_NUNCHAKU) && WPADProbe(pController->GetChannel(), &device) == 0 && device <= 2 &&
                WPADGetInfo(pController->GetChannel(), &info) == 0 && info.battery <= 1) {
                low = true;
                break;
            }
        }
    } else {
        IController* pController = IGameSession::GetInstance()->GetPlayer(player)->GetController();
        u32 device;
            WPADInfo info;
            if (pController && (pController->GetType() == IController::TYPE_WII_REMOTE ||
                                  pController->GetType() == IController::TYPE_CLASSIC ||
                                  pController->GetType() == IController::TYPE_NUNCHAKU) && WPADProbe(pController->GetChannel(), &device) == 0 && device <= 2 &&
                WPADGetInfo(pController->GetChannel(), &info) == 0 && info.battery <= 1) {
                low = true;
                
            }
    }
    pEngine->PushInt(low);
    return 0;
}

int CheckRemoteMemoryFatalError(NPpu::CEngine* pEngine) {
    int player = pEngine->PopInt();
    bool fatal = false;
    IController* pController = IGameSession::GetInstance()->GetPlayer(player)->GetController();
    if (pController && (pController->GetType() == IController::TYPE_NUNCHAKU ||
                        pController->GetType() == IController::TYPE_CLASSIC)) {
        pController = GetController(IController::TYPE_WII_REMOTE, pController->GetChannel());
        if (IGameSession::GetInstance()->IsControllerAssigned(pController)) {
            pController = nullptr;
        }
    }
    if (pController && pController->GetType() == IController::TYPE_WII_REMOTE) {
        fatal = IGameSession::GetInstance()->GetRemoteMemory(pController->GetChannel())->ConsumeFatalError();
    }
    pEngine->PushInt(fatal);
    return 0;
}

int GetButtonSymbol(NPpu::CEngine* pEngine) {
    u32 button = pEngine->PopInt();
    int player = pEngine->PopInt();
    IController* pController = GetConnectedController(player);
    pEngine->PushWString(pController ? pController->GetButtonSymbol(button) : L"");
    return 0;
}
