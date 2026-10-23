#include <game/MathUtil.h>
#include <game/luckyhelper.h>
#include <game/CBattleRoyalBox.h>
#include <algorithm>
#include <game/CPiiCollectionBox.h>
#include <game/IMiniMap.h>
#include <game/IMoneyLifeDisplay.h>
#include <game/CStatistic.h>
#include <game/INumericInput.h>
#include <game/IDlgCollection.h>
#include <game/IDlgInfo.h>
#include <game/StageDesc.h>
#include <game/COyaInfo.h>
#include <game/IPostEffect.h>
#include <game/SaveData.h>
#include <game/IGameStage.h>
#include <game/CSimpleGround.h>
#include <game/CDividedGround.h>
#include <game/ITutorialMsg.h>
#include <game/GameLoop.h>
#include <game/IBattleRoyalDisplay.h>
#include <game/IDlgPiiList.h>
#include <game/CFileData.h>
#include <game/BWorldLightSet.h>
#include <game/IDlgMsgWin.h>
#include <revolution/enc.h>
#include <game/IDlgPiiSelector.h>
#include <game/CNoticeDisplay.h>
#include <game/IGameEnding.h>
#include <game/IGameMap.h>
#include <game/BWorld.h>
#include <game/IGameSession.h>
#include <math.h>
#include <game/BWorldCamera.h>
#include <game/CPpuEngine.h>
static inline const char* PopString(NPpu::CEngine* pEngine) {
    NPpu::CValue value;
    pEngine->Pop(value);
    return pEngine->ToString(value);
}
std::tr1::shared_ptr<CFileData> LoadEventFile(CPpuEngine* pEngine, const char* pPath);



































bool IsStage7OddMap(MapIndex index);





















static wchar_t sMessageBuffer[1024];

static inline const wchar_t* GetMessageText(NPpu::CEngine* pEngine, const NPpu::CValue& rValue) {
    const wchar_t* pResult;
    if (rValue.IsString()) {
        const char* pText = pEngine->ToString(rValue);
        s32 size;
        ENCConvertStringSjisToUnicode(nullptr, &size, reinterpret_cast<const u8*>(pText), nullptr);
        if (size > 1023) { size = 1023; }
        sMessageBuffer[size] = L'\0';
        ENCConvertStringSjisToUnicode(reinterpret_cast<u16*>(sMessageBuffer), &size, reinterpret_cast<const u8*>(pText), nullptr);
        pResult = sMessageBuffer;
    } else {
        pResult = rValue.IsWString() ? pEngine->ToWString(rValue) : L"???";
    }
    return pResult;
}
static inline const wchar_t* PopMessageText(NPpu::CEngine* pEngine) {
    const wchar_t* pResult;
    NPpu::CValue value;
    pEngine->Pop(value);
    if (value.IsString()) {
        const char* pText = pEngine->ToString(value);
        s32 size;
        ENCConvertStringSjisToUnicode(nullptr, &size, reinterpret_cast<const u8*>(pText), nullptr);
        if (size > 1023) { size = 1023; }
        sMessageBuffer[size] = L'\0';
        ENCConvertStringSjisToUnicode(reinterpret_cast<u16*>(sMessageBuffer), &size, reinterpret_cast<const u8*>(pText), nullptr);
        pResult = sMessageBuffer;
    } else {
        pResult = value.IsWString() ? pEngine->ToWString(value) : L"???";
    }
    return pResult;
}























































































#include <game/CPpuEngine.h>
#include <game/IGameMap.h>
#include <game/CMsgBalloon.h>
#include <game/IComboDisplay.h>
#include <game/IEnemyInfo.h>

int LoadEventData(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::string name(PopString(pEngine));
    std::string path = "5:/event/" + name + ".arc";
    std::tr1::shared_ptr<CFileData> file = LoadEventFile(pEngine, path.c_str());
    if (file && file->IsLoaded()) {
        BWorld* pWorld = pEngine->GetWorld();
        if (pWorld) {
            BWorldCamera* pCamera = pWorld->GetCurrentCamera();
            if (pCamera) { pCamera->Load((path + "/event.brres").c_str(), false); }
            std::vector<BWorldObj*> objects = pWorld->GetObjects(BWorldLightSet::TYPE);
            for (std::vector<BWorldObj*>::iterator it = objects.begin(); it != objects.end(); ++it) {
                BWorldLightSet* pLight = DynamicCast<BWorldLightSet>(*it);
                pLight->Load((path + "/event.brres").c_str(), false);
            }
        }
        pEngine->PushInt(true);
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

std::tr1::shared_ptr<CFileData> LoadEventFile(CPpuEngine* pEngine, const char* pPath) {
    return pEngine->LoadFile(pPath, 0);
}

int LoadEventPlacementData(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    const char* pNodePrefix = PopString(pEngine);
    const char* pAnmName = PopString(pEngine);
    std::string name(PopString(pEngine));
    std::string path = "5:/event/" + name + ".arc/event.brres";
    pEngine->PushInt(pEngine->LoadPlacement(path.c_str(), pAnmName, pNodePrefix, 0));
    return 0;
}

int GetEventPlacementMtx(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    NPpu::CValue* pValues = pEngine->PopReference();
    u32 index = pEngine->PopInt();
    if (index < pEngine->GetPlacementCount()) {
        const nw4r::math::MTX34& rMatrix = pEngine->GetPlacement(index);
        for (int i = 0; i < 12; ++i) {
            pValues[i].SetFloat(rMatrix.a[i]);
        }
        pEngine->PushInt(true);
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

int ShowEpilogueCongratulations(NPpu::CEngine* pEngine) {
    dynamic_cast<IGameEnding*>(GetCurrentGameMap())->ShowCongratulations();
    pEngine->PushNull();
    return 0;
}

int HideEpilogueCongratulations(NPpu::CEngine* pEngine) {
    dynamic_cast<IGameEnding*>(GetCurrentGameMap())->StopCongratulations();
    pEngine->PushNull();
    return 0;
}

int AddStaffRollText(NPpu::CEngine* pEngine) {
    const wchar_t* pText = pEngine->PopWString();
    dynamic_cast<IGameEnding*>(GetCurrentGameMap())->ParseStaffRollText(pText);
    pEngine->PushNull();
    return 0;
}

int ShowStaffRoll(NPpu::CEngine* pEngine) {
    dynamic_cast<IGameEnding*>(GetCurrentGameMap())->StartStaffRoll();
    pEngine->PushNull();
    return 0;
}

int HideStaffRoll(NPpu::CEngine* pEngine) {
    dynamic_cast<IGameEnding*>(GetCurrentGameMap())->HideStaffRoll();
    pEngine->PushNull();
    return 0;
}

int IsStaffRollEnd(NPpu::CEngine* pEngine) {
    IGameEnding* pEnding = dynamic_cast<IGameEnding*>(GetCurrentGameMap());
    pEngine->PushInt(!pEnding || pEnding->IsStaffRollFinished());
    return 0;
}

int ShowEpilogueNext(NPpu::CEngine* pEngine) {
    dynamic_cast<IGameEnding*>(GetCurrentGameMap())->ShowNext();
    pEngine->PushNull();
    return 0;
}

int HideEpilogueNext(NPpu::CEngine* pEngine) {
    dynamic_cast<IGameEnding*>(GetCurrentGameMap())->StopNext();
    pEngine->PushNull();
    return 0;
}

int IsEpilogueNextShown(NPpu::CEngine* pEngine) {
    IGameEnding* pEnding = dynamic_cast<IGameEnding*>(GetCurrentGameMap());
    pEngine->PushInt(!pEnding || pEnding->IsNextVisible());
    return 0;
}

int GetLuckyNumber(NPpu::CEngine* pEngine) {
    NPpu::CValue* pValues = pEngine->PopReference();
    u64 number = 0;
    for (int i = 0; i < 6; ++i) {
        number = (number << 8) | GetSaveData(-1)->mData.mWirelessMacAddress[i];
    }
    number = number * 3 + GetCurrentSlot();
    u16 digits[5] = {};
    digits[0] = (number / 10000 / 10000 / 10000) % 10000;
    digits[1] = (number / 10000 / 10000) % 10000;
    digits[2] = (number / 10000) % 10000;
    digits[3] = number % 10000;
    ScrambleLuckyNumber(digits);
    digits[4] = CalcLuckyNumberCheck(digits, gLuckyNumberTable);
    for (int i = 0; i < 5; ++i) { pValues[i].SetInt(digits[i]); }
    pEngine->PushNull();
    return 0;
}

int GetCurrentMID(NPpu::CEngine* pEngine) {
    const MapIndex& rIndex = GetCurrentGameMap()->GetMapDesc().mIndex;
    pEngine->PushInt(*reinterpret_cast<const u32*>(&rIndex));
    return 0;
}

#pragma push
#pragma opt_dead_assignments off
int MakeMapID(NPpu::CEngine* pEngine) {
    u8 map = pEngine->PopInt();
    int stage = pEngine->PopInt();
    int area = pEngine->PopInt();
    int mode = pEngine->PopInt();
    MapIndex index = {};
    index.mMode = mode;
    index.mArea = area;
    index.mStage = stage;
    index.mMap = map;
    pEngine->PushInt(*reinterpret_cast<int*>(&index));
    return 0;
}
#pragma pop

int GetMapIndex(NPpu::CEngine* pEngine) {
    int value = pEngine->PopInt();
    MapIndex index = *reinterpret_cast<const MapIndex*>(&value);
    pEngine->PushInt(index.mMap);
    return 0;
}

int IsBattleRoyal(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        pEngine->PushInt(IsStage7EvenMap(pMap->GetMapDesc().mIndex));
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

int IsLobby(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        pEngine->PushInt(IsStage7OddMap(pMap->GetMapDesc().mIndex));
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

int ShowNoticeDisplay(NPpu::CEngine* pEngine) {
    int type = pEngine->PopInt();
    GetCurrentGameMap()->GetNoticeDisplay()->Show(type);
    pEngine->PushNull();
    return 0;
}

int HideNoticeDisplay(NPpu::CEngine* pEngine) {
    GetCurrentGameMap()->GetNoticeDisplay()->Close();
    pEngine->PushNull();
    return 0;
}

int IsNoticeDisplayShown(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetCurrentGameMap()->GetNoticeDisplay()->IsShown());
    return 0;
}

int GetCurrentNoticeDisplayType(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetCurrentGameMap()->GetNoticeDisplay()->GetType());
    return 0;
}

int AddEnemyToNoticeDisplay(NPpu::CEngine* pEngine) {
    int value = pEngine->PopInt();
    const PiiSpeciesData* pSpecies = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    GetCurrentGameMap()->GetNoticeDisplay()->AddEnemy(pSpecies, value);
    pEngine->PushNull();
    return 0;
}

int SetNoticeDisplayMoney(NPpu::CEngine* pEngine) {
    int money = pEngine->PopInt();
    GetCurrentGameMap()->GetNoticeDisplay()->SetWinner(money);
    pEngine->PushNull();
    return 0;
}

int SetNoticeDisplayTime(NPpu::CEngine* pEngine) {
    float time = pEngine->PopFloat();
    GetCurrentGameMap()->GetNoticeDisplay()->SetWinnerParam(time);
    pEngine->PushNull();
    return 0;
}

int ShowMiniMap(NPpu::CEngine* pEngine) {
    bool show = pEngine->PopInt() != 0;
    IMiniMap* pMap = GetCurrentGameMap()->GetMiniMap();
    if (pMap) {
        if (show) { pMap->Show(); }
        else { pMap->Hide(); }
    }
    pEngine->PushNull();
    return 0;
}

int ShowDlgPiiSelector(NPpu::CEngine* pEngine) {
    bool show = pEngine->PopInt() != 0;
    int player = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->GetDlgPiiSelector()->Show(player, show); }
    pEngine->PushNull();
    return 0;
}

int IsDlgPiiSelectorShown(NPpu::CEngine* pEngine) {
    int player = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        pEngine->PushInt(pMap->GetDlgPiiSelector()->IsShown(player));
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

int ShowMultiPlayerEntrance(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->GetDlgPiiSelector()->ShowMultiPlayerEntrance(); }
    pEngine->PushNull();
    return 0;
}

int IsMultiPlayerEntranceShown(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        pEngine->PushInt(pMap->GetDlgPiiSelector()->IsMultiPlayerEntranceShown());
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

int IsReplacePiiAvailable(NPpu::CEngine* pEngine) {
    int player = pEngine->PopInt();
    if (GetActiveGameMap()) {
        pEngine->PushInt(IGameSession::GetInstance()->IsReplacePiiAvailable(player));
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

int MsgWin_ShowMessage(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IDlgMsgWin* pWindow = pMap ? pMap->GetDlgMsgWin() : nullptr;
    if (pWindow) {
        const wchar_t* pText = PopMessageText(pEngine);
        pWindow->OpenMessage(pText);
        pEngine->PushInt(true);
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

int OpenScriptMessage(NPpu::CEngine* pEngine, bool player, bool image) {
    IGameMap* pMap = GetActiveGameMap();
    IDlgMsgWin* pWindow = pMap ? pMap->GetDlgMsgWin() : nullptr;
    if (pWindow) {
        const char* pImageName = nullptr;
        if (image) { pImageName = PopString(pEngine); }
        const wchar_t* pText = PopMessageText(pEngine);
        int slot = -1;
        if (player) { slot = pEngine->PopInt(); }
        pWindow->SetControllerSlot(slot);
        if (image) { pWindow->OpenImageMessage(pText, pImageName); }
        else { pWindow->OpenMessage(pText); }
        pEngine->PushInt(true);
    } else {
        pEngine->Drop(player + image + 1);
        pEngine->PushInt(false);
    }
    return 0;
}

int MessageOpen(NPpu::CEngine* pEngine) {
    return OpenScriptMessage(pEngine, false, false);
}

int PlayerMessageOpen(NPpu::CEngine* pEngine) {
    return OpenScriptMessage(pEngine, true, false);
}

int MessageOpenWithImage(NPpu::CEngine* pEngine) {
    return OpenScriptMessage(pEngine, false, true);
}

int PlayerMessageOpenWithImage(NPpu::CEngine* pEngine) {
    return OpenScriptMessage(pEngine, true, true);
}

int MessageIsClosed(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IDlgMsgWin* pWindow = pMap ? pMap->GetDlgMsgWin() : nullptr;
    pEngine->PushInt(pWindow ? pWindow->IsMessageClosed() : true);
    return 0;
}

int OpenScriptYesNo(NPpu::CEngine* pEngine, bool player, bool withDefault) {
    IGameMap* pMap = GetActiveGameMap();
    IDlgMsgWin* pWindow = pMap ? pMap->GetDlgMsgWin() : nullptr;
    if (pWindow) {
        if (withDefault) { pWindow->SetDefaultYes(pEngine->PopInt() != 0); }
        const wchar_t* pText = PopMessageText(pEngine);
        int slot = -1;
        if (player) { slot = pEngine->PopInt(); }
        pWindow->SetControllerSlot(slot);
        pWindow->OpenYesNo(pText);
        pEngine->PushInt(true);
    } else {
        pEngine->Drop(player + withDefault + 1);
        pEngine->PushInt(false);
    }
    return 0;
}

int YesNoOpen(NPpu::CEngine* pEngine) {
    return OpenScriptYesNo(pEngine, false, false);
}

int PlayerYesNoOpen(NPpu::CEngine* pEngine) {
    return OpenScriptYesNo(pEngine, true, false);
}

int YesNoOpenWithDefault(NPpu::CEngine* pEngine) {
    return OpenScriptYesNo(pEngine, false, true);
}

int PlayerYesNoOpenWithDefault(NPpu::CEngine* pEngine) {
    return OpenScriptYesNo(pEngine, true, true);
}

int YesNoIsClosed(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IDlgMsgWin* pWindow = pMap ? pMap->GetDlgMsgWin() : nullptr;
    pEngine->PushInt(pWindow ? pWindow->IsYesNoClosed() : true);
    return 0;
}

int YesNoGetResult(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IDlgMsgWin* pWindow = pMap ? pMap->GetDlgMsgWin() : nullptr;
    pEngine->PushInt(pWindow ? pWindow->IsYesSelected() : false);
    return 0;
}

static inline void SetScriptOptions(IDlgMsgWin* pWindow, NPpu::CEngine* pEngine, int itemArgs) {
    for (int i = 0; i < itemArgs / 2; ++i) {
        NPpu::CValue label;
        NPpu::CValue text;
        pEngine->GetArg(-1 - (i * 2 - itemArgs), label);
        pEngine->GetArg(itemArgs - 2 - i * 2, text);
        std::wstring optionLabel(GetMessageText(pEngine, label));
        pWindow->SetOption(i, optionLabel.c_str(), GetMessageText(pEngine, text));
    }
}

static inline void OpenOptionsWithTitle(IDlgMsgWin* pWindow, NPpu::CEngine* pEngine, const NPpu::CValue& rTitle) {
    pWindow->OpenOptions(GetMessageText(pEngine, rTitle));
}

int OpenScriptChoices(NPpu::CEngine* pEngine, bool withDefault, bool withItemCount) {
    int argCount = pEngine->FetchArgCount();
    IGameMap* pMap = GetActiveGameMap();
    IDlgMsgWin* pWindow = pMap ? pMap->GetDlgMsgWin() : nullptr;
    if (pWindow) {
        int player = -1;
        int itemArgs = argCount - 1;
        int argIndex = argCount - 1;
        NPpu::CValue title;
        pEngine->GetArg(argIndex, title);
        if (title.IsInt()) {
            player = title.ToInt();
            --argIndex;
            --itemArgs;
            pEngine->GetArg(argIndex, title);
        }
        if (withDefault) { --itemArgs; }
        if (withItemCount) { --itemArgs; }
        if (itemArgs >= 2 && itemArgs % 2 == 0) {
            if (withDefault) {
                NPpu::CValue value;
                pEngine->GetArg(--argIndex, value);
                pWindow->SetDefaultOption(value.ToInt());
            }
            if (withItemCount) {
                NPpu::CValue value;
                pEngine->GetArg(--argIndex, value);
                int dropped = argIndex - 2 * value.ToInt();
                if (dropped < 0 || dropped >= itemArgs) { dropped = 0; }
                argCount -= dropped;
                itemArgs -= dropped;
                pEngine->Drop(dropped);
            }
            SetScriptOptions(pWindow, pEngine, itemArgs);
            pWindow->SetControllerSlot(player);
            OpenOptionsWithTitle(pWindow, pEngine, title);
        }
    }
    pEngine->Drop(argCount);
    pEngine->PushNull();
    return 0;
}

int ChoiceOpenWithDefaultAndItemNum(NPpu::CEngine* pEngine) {
    return OpenScriptChoices(pEngine, true, true);
}

int ChoiceOpen(NPpu::CEngine* pEngine) {
    return OpenScriptChoices(pEngine, false, false);
}

int ChoiceOpenWithDefault(NPpu::CEngine* pEngine) {
    return OpenScriptChoices(pEngine, true, false);
}

int ChoiceIsClosed(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IDlgMsgWin* pWindow = pMap ? pMap->GetDlgMsgWin() : nullptr;
    pEngine->PushInt(pWindow ? pWindow->IsOptionsClosed() : true);
    return 0;
}

int ChoiceGetResult(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IDlgMsgWin* pWindow = pMap ? pMap->GetDlgMsgWin() : nullptr;
    pEngine->PushInt(pWindow ? pWindow->GetSelectedOption() : 0);
    return 0;
}

int Pause(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->SetPausing(true); }
    pEngine->PushNull();
    return 0;
}

int Resume(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->SetPausing(false); }
    pEngine->PushNull();
    return 0;
}

int IsPausing(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        pEngine->PushInt(pMap->IsPausing());
    } else {
        pEngine->PushInt(true);
    }
    return 0;
}

int Hitstop(NPpu::CEngine* pEngine) {
    int frames = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->Hitstop(frames); }
    pEngine->PushNull();
    return 0;
}

int IsHitstopping(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        pEngine->PushInt(pMap->IsHitstopping());
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

int EnterEvent(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->EnterEvent(); }
    pEngine->PushNull();
    return 0;
}

int ExitEvent(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->ExitEvent(); }
    pEngine->PushNull();
    return 0;
}

int IsInEvent(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        pEngine->PushInt(pMap->IsInEvent());
    } else {
        pEngine->PushInt(false);
    }
    return 0;
}

int StartTimer(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int frames;
    int step = -1;
    if (pEngine->FetchArgCount() > 1) { step = pEngine->PopInt(); }
    frames = ceil(pEngine->PopFloat() * 60.0f);
    GetCurrentGameMap()->StartTimer(frames, step);
    pEngine->PushNull();
    return 0;
}

int IsTimerWorking(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetCurrentGameMap()->IsTimerWorking());
    return 0;
}

int AddTimer(NPpu::CEngine* pEngine) {
    int frames = ceil(pEngine->PopFloat() * 60.0f);
    GetCurrentGameMap()->AddTimer(frames);
    pEngine->PushNull();
    return 0;
}

int StopTimer(NPpu::CEngine* pEngine) {
    GetCurrentGameMap()->StopTimer();
    pEngine->PushNull();
    return 0;
}

int GetTimerSec(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(GetCurrentGameMap()->GetTimerFrame() / 60.0f);
    return 0;
}

int GetTimerFrame(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetCurrentGameMap()->GetTimerFrame());
    return 0;
}

int ShowBRDisplay(NPpu::CEngine* pEngine) {
    GetCurrentGameMap()->GetBattleRoyalDisplay()->Show();
    pEngine->PushNull();
    return 0;
}

int HideBRDisplay(NPpu::CEngine* pEngine) {
    GetCurrentGameMap()->GetBattleRoyalDisplay()->Hide();
    pEngine->PushNull();
    return 0;
}

int ShowDlgPiiList(NPpu::CEngine* pEngine) {
    s32 type = pEngine->PopInt();
    int player = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        pEngine->PushInt(pMap->GetDlgPiiList()->Show(player, type));
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int GetDlgPiiListType(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        pEngine->PushInt(pMap->GetDlgPiiList()->GetType());
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int ShowDlgCollection(NPpu::CEngine* pEngine) {
    pEngine->PopInt();
    int player = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pEngine->PushInt(pMap->GetDlgCollection()->Show(player)); }
    else { pEngine->PushInt(0); }
    return 0;
}

int IsDlgCollectionShown(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pEngine->PushInt(pMap->GetDlgCollection()->IsShown()); }
    else { pEngine->PushInt(0); }
    return 0;
}

int ShowDlgInfo(NPpu::CEngine* pEngine) {
    pEngine->PopInt();
    int player = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pEngine->PushInt(pMap->GetDlgInfo()->Show(player)); }
    else { pEngine->PushInt(0); }
    pEngine->PushNull();
    return 0;
}

int IsDlgInfoShown(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pEngine->PushInt(pMap->GetDlgInfo()->IsShown()); }
    else { pEngine->PushInt(0); }
    return 0;
}

int DlgInfoClear(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->GetDlgInfo()->Clear(); }
    pEngine->PushNull();
    return 0;
}

int DlgInfoSetTitle(NPpu::CEngine* pEngine) {
    const wchar_t* pTitle = pEngine->PopWString();
    int page = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->GetDlgInfo()->SetTitle(page, pTitle); }
    pEngine->PushNull();
    return 0;
}

int DlgInfoSetLine(NPpu::CEngine* pEngine) {
    std::wstring third(pEngine->PopWString());
    std::wstring second(pEngine->PopWString());
    std::wstring first(pEngine->PopWString());
    int line = pEngine->PopInt();
    int page = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->GetDlgInfo()->SetLine(page, line, first.c_str(), second.c_str(), third.c_str()); }
    pEngine->PushNull();
    return 0;
}

int DlgInfoSetText(NPpu::CEngine* pEngine) {
    const wchar_t* pText = pEngine->PopWString();
    int line = pEngine->PopInt();
    int page = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) { pMap->GetDlgInfo()->SetText(page, line, pText); }
    pEngine->PushNull();
    return 0;
}

int GetGameFrame(NPpu::CEngine* pEngine) {
    pEngine->PushInt(gGameFrame);
    return 0;
}

int TutorialMessageClear(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    ITutorialMsg* pMessage = pMap ? pMap->GetTutorialMsg() : nullptr;
    if (pMessage) { pMessage->Clear(); }
    pEngine->PushNull();
    return 0;
}

int TutorialMessage(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    ITutorialMsg* pMessage = pMap ? pMap->GetTutorialMsg() : nullptr;
    if (pMessage) {
        pMessage->Show(PopMessageText(pEngine));
    } else {
        pEngine->Drop(1);
    }
    pEngine->PushNull();
    return 0;
}

int TutorialMessageClose(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    ITutorialMsg* pMessage = pMap ? pMap->GetTutorialMsg() : nullptr;
    if (pMessage) { pMessage->Close(); }
    pEngine->PushNull();
    return 0;
}

int LoadGroundAttr(NPpu::CEngine* pEngine) {
    const char* pName = PopString(pEngine);
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        CDividedGround* pGround = DynamicCast<CDividedGround>(pMap->GetWorld()->GetGround());
        if (pGround) { pGround->LoadAttr(pName, false); }
    }
    pEngine->PushNull();
    return 0;
}

static inline void AddPoppedBalloon(IMsgBalloon* pBalloon, NPpu::CEngine* pEngine) {
    const wchar_t* pText = PopMessageText(pEngine);
    float x = pEngine->PopFloat();
    float z = pEngine->PopFloat();
    pBalloon->Add(pText, z, x);
}

int AddMessageBalloon(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IMsgBalloon* pBalloon = pMap ? pMap->GetMsgBalloon() : nullptr;
    if (pBalloon) {
        AddPoppedBalloon(pBalloon, pEngine);
    }
    pEngine->PushNull();
    return 0;
}

int RegisterMessageBalloon(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IMsgBalloon* pBalloon = pMap ? pMap->GetMsgBalloon() : nullptr;
    const wchar_t* pText = PopMessageText(pEngine);
    float radius = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pBalloon) { pBalloon->Register(pObject, pText, radius); }
    pEngine->PushNull();
    return 0;
}

int RegisterWarpzoneBalloon(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IMsgBalloon* pBalloon = pMap ? pMap->GetMsgBalloon() : nullptr;
    int stageValue = pEngine->PopInt();
    float radius = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pBalloon) {
        StageID stage;
        *reinterpret_cast<int*>(&stage) = stageValue;
        pBalloon->RegisterWarpzone(pObject, radius, stage);
    }
    pEngine->PushNull();
    return 0;
}

int ShowMessageBalloon(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IMsgBalloon* pBalloon = pMap ? pMap->GetMsgBalloon() : nullptr;
    bool show = pEngine->PopInt() != 0;
    if (pBalloon) { pBalloon->Show(show); }
    pEngine->PushNull();
    return 0;
}

int IsMessageBalloonShown(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IMsgBalloon* pBalloon = pMap != nullptr ? pMap->GetMsgBalloon() : nullptr;
    int index = pEngine->PopInt();
    bool shown = false;
    if (pBalloon != nullptr) { shown = pBalloon->IsShown(index); }
    pEngine->PushInt(shown);
    return 0;
}

int AdjustMessageBalloonY(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IMsgBalloon* pBalloon = pMap != nullptr ? pMap->GetMsgBalloon() : nullptr;
    f32 y = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pBalloon != nullptr) {
        pBalloon->AdjustY(pObject, nw4r::math::VEC3(0.0f, y, 0.0f));
    }
    pEngine->PushNull();
    return 0;
}

int ShowEnemyInfo(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IEnemyInfo* pInfo = pMap ? pMap->GetEnemyInfo() : nullptr;
    int frames = pEngine->PopInt();
    bool flash = pEngine->PopInt() != 0;
    std::tr1::shared_ptr<CPiiPersonalData> data = static_cast<CPpuEngine*>(pEngine)->PopPersonalData();
    if (pInfo && data) { pInfo->Show(data, flash, frames); }
    pEngine->PushNull();
    return 0;
}

int HideEnemyInfo(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IEnemyInfo* pInfo = pMap != nullptr ? pMap->GetEnemyInfo() : nullptr;
    if (pInfo != nullptr) {
        pInfo->Hide();
    }
    pEngine->PushNull();
    return 0;
}

int GetComboGauge(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IComboDisplay* pDisplay = pMap != nullptr ? pMap->GetComboDisplay() : nullptr;
    pEngine->PushInt(pDisplay != nullptr ? pDisplay->GetComboGauge() : 0);
    return 0;
}

int SetComboGauge(NPpu::CEngine* pEngine) {
    int value = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    IComboDisplay* pDisplay = pMap != nullptr ? pMap->GetComboDisplay() : nullptr;
    if (pDisplay != nullptr) {
        pDisplay->SetComboGauge(value);
    }
    pEngine->PushNull();
    return 0;
}

int GetComboCount(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IComboDisplay* pDisplay = pMap != nullptr ? pMap->GetComboDisplay() : nullptr;
    pEngine->PushInt(pDisplay != nullptr ? pDisplay->GetComboCount() : 0);
    return 0;
}

int SetComboCount(NPpu::CEngine* pEngine) {
    int value = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    IComboDisplay* pDisplay = pMap != nullptr ? pMap->GetComboDisplay() : nullptr;
    if (pDisplay != nullptr) {
        pDisplay->SetComboCount(value);
    }
    pEngine->PushNull();
    return 0;
}

int ShowComboCount(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IComboDisplay* pDisplay = pMap != nullptr ? pMap->GetComboDisplay() : nullptr;
    if (pDisplay != nullptr) {
        pDisplay->ShowComboCount();
    }
    pEngine->PushNull();
    return 0;
}

int ShowMoneyLifeDisplay(NPpu::CEngine* pEngine) {
    GetCurrentGameMap()->GetMoneyLifeDisplay()->Show();
    pEngine->PushNull();
    return 0;
}

int HideMoneyLifeDisplay(NPpu::CEngine* pEngine) {
    GetCurrentGameMap()->GetMoneyLifeDisplay()->Hide();
    pEngine->PushNull();
    return 0;
}

int ForceShowMoneyLifeDisplay(NPpu::CEngine* pEngine) {
    int show = pEngine->PopInt();
    GetCurrentGameMap()->GetMoneyLifeDisplay()->SetForceVisible(show != 0);
    pEngine->PushNull();
    return 0;
}

int DisappearModelObjs(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap != nullptr) {
        pMap->DisappearModelObjs();
    }
    pEngine->PushNull();
    return 0;
}

int AppearModelObjs(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap != nullptr) {
        pMap->AppearModelObjs();
    }
    pEngine->PushNull();
    return 0;
}

int OpenNumericInputScreen(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    INumericInput* pInput = pMap ? pMap->GetNumericInput() : nullptr;
    int count = pEngine->FetchArgCount();
    if (pInput && count <= 1) {
        int player = -1;
        if (count == 1) { player = pEngine->PopInt(); }
        pInput->Start(player);
        pEngine->PushInt(1);
    } else {
        pEngine->Drop(count);
        pEngine->PushInt(0);
    }
    return 0;
}

int IsNumericInputScreenClosed(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    INumericInput* pInput = pMap ? pMap->GetNumericInput() : nullptr;
    pEngine->PushInt(pInput ? pInput->IsFinished() : true);
    return 0;
}

int GetNumericInputScreenResult(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    INumericInput* pInput = pMap ? pMap->GetNumericInput() : nullptr;
    pEngine->PushInt(pInput ? pInput->IsConfirmed() : true);
    return 0;
}

int GetNumericInputScreenCode(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    INumericInput* pInput = pMap ? pMap->GetNumericInput() : nullptr;
    const wchar_t* pCode = pEngine->AllocWString(pInput ? pInput->GetText().c_str() : L"");
    pEngine->PushWString(pCode);
    return 0;
}

int SetEnemyCount(NPpu::CEngine* pEngine) {
    int count = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    IBattleRoyalDisplay* pDisplay = pMap ? pMap->GetBattleRoyalDisplay() : nullptr;
    if (pDisplay) { pDisplay->SetEnemyCount(count); }
    pEngine->PushNull();
    return 0;
}

int DecEnemyCount(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IBattleRoyalDisplay* pDisplay = pMap ? pMap->GetBattleRoyalDisplay() : nullptr;
    if (pDisplay) { pDisplay->DecEnemyCount(); }
    pEngine->PushNull();
    return 0;
}

int GetEnemyCount(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    IBattleRoyalDisplay* pDisplay = pMap ? pMap->GetBattleRoyalDisplay() : nullptr;
    pEngine->PushInt(pDisplay ? pDisplay->GetEnemyCount() : 0);
    return 0;
}

int ShowPostEffect(NPpu::CEngine* pEngine) {
    int effect = pEngine->PopInt();
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        IPostEffect* pEffect = pMap->GetPostEffect();
        if (pEffect) { pEffect->Show(effect); }
    }
    pEngine->PushNull();
    return 0;
}

int HidePostEffect(NPpu::CEngine* pEngine) {
    IGameMap* pMap = GetActiveGameMap();
    if (pMap) {
        IPostEffect* pEffect = pMap->GetPostEffect();
        if (pEffect) { pEffect->Hide(); }
    }
    pEngine->PushNull();
    return 0;
}

int GetMoney(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IGameSession::GetInstance()->GetMoney());
    return 0;
}

int SetMoney(NPpu::CEngine* pEngine) {
    int money = pEngine->PopInt();
    IGameSession::GetInstance()->SetMoney(money);
    pEngine->PushNull();
    return 0;
}

int AddMoney(NPpu::CEngine* pEngine) {
    int money = pEngine->PopInt();
    IGameSession::GetInstance()->AddMoney(money);
    pEngine->PushNull();
    return 0;
}

int GetTicketMonsno(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetSaveData(-1)->mData.mTicketMonsno);
    return 0;
}

int GetTicketFormno(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetSaveData(-1)->mData.mTicketFormno);
    return 0;
}

int SetTicket(NPpu::CEngine* pEngine) {
    u8 formno = pEngine->PopInt();
    u16 monsno = pEngine->PopInt();
    GetSaveData(-1)->mData.mTicketMonsno = monsno;
    GetSaveData(-1)->mData.mTicketFormno = formno;
    pEngine->PushNull();
    return 0;
}

int BeginStage(NPpu::CEngine* pEngine) {
    MapIndex index;
    *reinterpret_cast<int*>(&index) = pEngine->PopInt();
    IGameSession::GetInstance()->RequestMapChange(index, 0, 0);
    pEngine->PushNull();
    return 0;
}

int EndStage(NPpu::CEngine* pEngine) {
    int frames = pEngine->PopInt();
    int type = pEngine->PopInt();
    MapIndex index;
    *reinterpret_cast<int*>(&index) = pEngine->PopInt();
    IGameSession::GetInstance()->RequestMapChange(index, type, frames);
    pEngine->PushNull();
    return 0;
}

int SetFlag(NPpu::CEngine* pEngine) {
    SetFlag(pEngine->PopInt(), 1);
    pEngine->PushNull();
    return 0;
}

int ClearFlag(NPpu::CEngine* pEngine) {
    SetFlag(pEngine->PopInt(), 0);
    pEngine->PushNull();
    return 0;
}

int ModifyFlag(NPpu::CEngine* pEngine) {
    int value = pEngine->PopInt();
    int flag = pEngine->PopInt();
    SetFlag(flag, value != 0);
    pEngine->PushNull();
    return 0;
}

int CheckFlag(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IsGameFlagSet(pEngine->PopInt()));
    return 0;
}

int NANDSaveTestAsync(NPpu::CEngine* pEngine) {
    IGameSession::GetInstance()->StartMapTransitionOut();
    pEngine->PushNull();
    return 0;
}

int NANDisTaskCompleted(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IGameSession::GetInstance()->IsMapChangeFinished());
    return 0;
}

int GetMaxGrade(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetSaveData(-1)->mData.mFlags[0]);
    return 0;
}

int GetMaxDifficulty(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetSaveData(-1)->mData.mFlags[1]);
    return 0;
}

int SetMaxGrade(NPpu::CEngine* pEngine) {
    s8 value = pEngine->PopInt();
    GetSaveData(-1)->mData.mFlags[0] = value;
    pEngine->PushNull();
    return 0;
}

int SetMaxDifficulty(NPpu::CEngine* pEngine) {
    s8 value = pEngine->PopInt();
    GetSaveData(-1)->mData.mFlags[1] = value;
    pEngine->PushNull();
    return 0;
}

int GetStatValue(NPpu::CEngine* pEngine) {
    int id = pEngine->PopInt();
    pEngine->PushInt(IGameSession::GetInstance()->GetStatistic()->Get(id));
    return 0;
}

int SetStatValue(NPpu::CEngine* pEngine) {
    int value = pEngine->PopInt();
    int id = pEngine->PopInt();
    IGameSession::GetInstance()->GetStatistic()->Set(id, value);
    pEngine->PushNull();
    return 0;
}

int AddStatValue(NPpu::CEngine* pEngine) {
    int value = pEngine->PopInt();
    int id = pEngine->PopInt();
    IGameSession::GetInstance()->GetStatistic()->Add(id, value);
    pEngine->PushNull();
    return 0;
}

int SetStatCounterStopValue(NPpu::CEngine* pEngine) {
    int value = pEngine->PopInt();
    int id = pEngine->PopInt();
    IGameSession::GetInstance()->GetStatistic()->SetCounterStopValue(id, value);
    pEngine->PushNull();
    return 0;
}

int IsWazaUsed(NPpu::CEngine* pEngine) {
    int waza = pEngine->PopInt();
    pEngine->PushInt(IGameSession::GetInstance()->GetStatistic()->IsWazaUsed(waza));
    return 0;
}

int SetWazaUsed(NPpu::CEngine* pEngine) {
    int used = pEngine->PopInt();
    int waza = pEngine->PopInt();
    IGameSession::GetInstance()->GetStatistic()->SetWazaUsed(waza, used != 0);
    pEngine->PushNull();
    return 0;
}

int GetWazaUsedCount(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IGameSession::GetInstance()->GetStatistic()->GetWazaUsedCount());
    return 0;
}

static inline CMapRecord& GetSavedMapRecord(const MapIndex& rIndex) {
    int mode = rIndex.mMode;
    int area = rIndex.mArea;
    int stage = rIndex.mStage;
    return GetSaveData(-1)->mData.mMapRecords[mode][area][stage][rIndex.mMap];
}

int MapStatus_allFlags(NPpu::CEngine* pEngine) {
    int flags = pEngine->PopInt();
    MapIndex index;
    *reinterpret_cast<int*>(&index) = pEngine->PopInt();
    pEngine->PushInt(GetSavedMapRecord(index).IsFlagSet(flags));
    return 0;
}

int MapStatus_setFlags(NPpu::CEngine* pEngine) {
    int flags = pEngine->PopInt();
    MapIndex index;
    *reinterpret_cast<int*>(&index) = pEngine->PopInt();
    GetSavedMapRecord(index).SetFlags(flags);
    pEngine->PushNull();
    return 0;
}

int GetMapIndexSize(NPpu::CEngine* pEngine) {
    MapIndex index;
    *reinterpret_cast<int*>(&index) = pEngine->PopInt();
    pEngine->PushInt(IGameSession::GetInstance()->GetStageDesc(index)->mMapDescs.size());
    return 0;
}

int IsStageCleared(NPpu::CEngine* pEngine) {
    MapIndex index;
    *reinterpret_cast<int*>(&index) = pEngine->PopInt();
    index.mMap = IGameSession::GetInstance()->GetStageDesc(index)->mMapDescs.size() - 1;
    int stage;
    int area;
    int mode;
    mode = index.mMode;
    area = index.mArea;
    stage = index.mStage;
    pEngine->PushInt(GetSaveData(-1)->mData.mMapRecords[mode][area][stage][index.mMap].IsFlagSet(2));
    return 0;
}

int GetBattleRoyalClearSec(NPpu::CEngine* pEngine) {
    int area = pEngine->PopInt();
    int mode = pEngine->PopInt();
    std::tr1::shared_ptr<CBattleRoyalData> data = IGameSession::GetInstance()->GetBattleRoyalBox()->GetData(mode, area);
    pEngine->PushFloat(data->GetBestTime());
    return 0;
}

int GetTotalPlaySec(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(IGameSession::GetInstance()->GetPlayTimeSeconds());
    return 0;
}

int GetOyaID(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IGameSession::GetInstance()->GetOyaInfo()->GetCount());
    return 0;
}

int GetManyDefeatedPii(NPpu::CEngine* pEngine) {
    int count;
    NPpu::CValue* pValues = pEngine->PopArray(&count);
    std::vector<u16> monsnos = IGameSession::GetInstance()->GetPiiCollectionBox()->GetManyDefeatedPii();
    for (unsigned int i = 0; i < count; ++i) {
        if (monsnos.size() <= i) {
            pValues[i].SetInt(0);
        } else {
            pValues[i].SetInt(monsnos[i]);
        }
    }
    pEngine->PushInt(monsnos.size());
    return 0;
}

int GetExtraBR_CurrentRound(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetSaveData(-1)->mData.mExtraBRCurrentRound);
    return 0;
}

int SetExtraBR_CurrentRound(NPpu::CEngine* pEngine) {
    s8 value = pEngine->PopInt();
    GetSaveData(-1)->mData.mExtraBRCurrentRound = value;
    pEngine->PushNull();
    return 0;
}

int GetExtraBR_MaxRound(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetSaveData(-1)->mData.mExtraBRMaxRound);
    return 0;
}

int SetExtraBR_MaxRound(NPpu::CEngine* pEngine) {
    s8 value = pEngine->PopInt();
    GetSaveData(-1)->mData.mExtraBRMaxRound = value;
    pEngine->PushNull();
    return 0;
}

int IsBattleRoyalFirstHalf(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IsBattleRoyalFirstPart(GetCurrentGameMap()->GetMapDesc().mIndex));
    return 0;
}

int GetStageClearedNum(NPpu::CEngine* pEngine) {
    MapIndex index;
    *reinterpret_cast<int*>(&index) = pEngine->PopInt();
    int map = IGameSession::GetInstance()->GetStageDesc(index)->mMapDescs.size() - 1;
    int count = GetSaveData(-1)->mData.mMapRecords[index.mMode][index.mArea][index.mStage][map].GetClearCount();
    pEngine->PushInt(Clamp(count, 0, 999));
    return 0;
}

#pragma push
#pragma opt_common_subs off
int IncMapClearedNum(NPpu::CEngine* pEngine) {
    MapIndex index;
    *reinterpret_cast<int*>(&index) = pEngine->PopInt();
    GetSaveData(-1)->mData.mMapRecords[index.mMode][index.mArea][index.mStage][index.mMap].IncrementClearCount();
    pEngine->PushNull();
    return 0;
}
#pragma pop

int DEBUG_GetDropRareFlag(NPpu::CEngine* pEngine) {
    pEngine->PushInt(0);
    return 0;
}

int DEBUG_GetFixedPopPokemonMonsno(NPpu::CEngine* pEngine) {
    pEngine->PushInt(0);
    return 0;
}

int DEBUG_GetFixedPopPokemonParaSex(NPpu::CEngine* pEngine) {
    pEngine->PushInt(2);
    return 0;
}

int DEBUG_NotifyExtraSelectedStage(NPpu::CEngine* pEngine) {
    pEngine->PopInt();
    pEngine->PopInt();
    pEngine->PopInt();
    pEngine->PushNull();
    return 0;
}

int GetStageDifficulty(NPpu::CEngine* pEngine) {
    StageID stage;
    stage.mValue = pEngine->PopInt();
    pEngine->PushInt(stage.mKind);
    return 0;
}

int GetStageGrade(NPpu::CEngine* pEngine) {
    StageID stage;
    stage.mValue = pEngine->PopInt();
    pEngine->PushInt(stage.mRank);
    return 0;
}

int GetStageType(NPpu::CEngine* pEngine) {
    StageID stage;
    stage.mValue = pEngine->PopInt();
    pEngine->PushInt(stage.mWorld);
    return 0;
}

int MakeStageID(NPpu::CEngine* pEngine) {
    int type = pEngine->PopInt();
    int grade = pEngine->PopInt();
    int difficulty = pEngine->PopInt();
    StageID stage = {};
    stage.mKind = difficulty;
    stage.mRank = grade;
    stage.mWorld = type;
    pEngine->PushInt(stage.mValue);
    return 0;
}

int GetLife(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetGameStage()->GetLife());
    return 0;
}

int SetLife(NPpu::CEngine* pEngine) {
    int life = pEngine->PopInt();
    GetGameStage()->SetLife(life);
    pEngine->PushNull();
    return 0;
}

int ChangeMap(NPpu::CEngine* pEngine) {
    MapIndex index;
    *reinterpret_cast<int*>(&index) = pEngine->PopInt();
    GetGameStage()->ChangeMap(index);
    pEngine->PushNull();
    return 0;
}

int SetEnvSE(NPpu::CEngine* pEngine) {
    int id = pEngine->PopInt();
    GetGameStage()->SetEnvSE(id);
    pEngine->PushNull();
    return 0;
}

int StopEnvSE(NPpu::CEngine* pEngine) {
    GetGameStage()->StopEnvSE();
    pEngine->PushNull();
    return 0;
}

int NewSimpleGround(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int count = pEngine->FetchArgCount();
    BWorldObj* pParent = nullptr;
    if (count >= 2) { pParent = pEngine->PopWorldObj(); }
    const char* pName = nullptr;
    if (count >= 1) {
        NPpu::CValue value;
        pEngine->Pop(value);
        if (value.IsString()) { pName = pEngine->ToString(value); }
    }
    std::tr1::shared_ptr<CSimpleGround> ground;
    ground.reset(new CSimpleGround(CSimpleGround::TYPE));
    ground->SwapRegister(pEngine->GetRegister());
    ground->Initialize(false);
    if (pParent) { ground->SetParent(pParent->shared_from_this()); }
    if (pEngine->GetWorld()->AddGround(ground)) {
        if (pName) { ground->LoadScript(pName, 0, std::tr1::shared_ptr<CPpuEnv>()); }
        pEngine->PushInt(ground->GetId());
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_CSimpleGround_loadModel(NPpu::CEngine* pEngine) {
    const char* pName = PopString(pEngine);
    CSimpleGround* pGround = DynamicCast<CSimpleGround>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pGround) { pGround->LoadModel(pName, false); }
    pEngine->PushNull();
    return 0;
}
