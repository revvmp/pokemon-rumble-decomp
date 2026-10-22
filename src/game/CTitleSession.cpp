#include <game/ITitleSession.h>

#include <game/BUILayout.h>
#include <game/CControllerComposition.h>
#include <game/CFiber.h>
#include <game/CUIManager.h>
#include <game/ControllerTable.h>
#include <game/FileList.h>
#include <game/IDlgMsgWin.h>
#include <game/ISaveManager.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/MessageFormat.h>
#include <game/SaveData.h>
#include <game/Sound.h>
#include <revolution/net.h>
#include <revolution/os.h>

#include <memory>
#include <string>
#include <vector>

void ChangeBGM(u32 id);
IUIInput* GetUIInput();

namespace {

enum Screen {
    SCREEN_TITLE,
    SCREEN_NEW_GAME,
    SCREEN_CONTINUE,
    SCREEN_DIFFICULTY,
    SCREEN_OPTIONS,
    SCREEN_VIBRATION,
    SCREEN_DELETE,
    SCREEN_COUNT,
    SCREEN_NONE = SCREEN_COUNT
};

enum {
    BUTTON_COUNT = 4,
    BUTTON_BACK = 3,
    SLOT_COUNT = 3,
    BGM_TITLE = 624
};

enum {
    INPUT_UP = 0x1,
    INPUT_RIGHT = 0x2,
    INPUT_DOWN = 0x4,
    INPUT_LEFT = 0x8,
    INPUT_CANCEL = 0x10,
    INPUT_DECIDE = 0x20
};

struct TitleText {
    int mScreen;
    const char* mPane;
    int mMessageId;
    const wchar_t* mDefault;
};

const char* sButtonPanes[BUTTON_COUNT] = {"BUTTON_01", "BUTTON_02", "BUTTON_03", "BUTTON_BACK"};
const char* sSlotTextPanes[SLOT_COUNT] = {"TEXT_1D", "TEXT_2D", "TEXT_3D"};
const char* sScreenLayouts[SCREEN_COUNT] = {"title_main",   "title_tududki",    "title_tududki", "title_difficulty",
                                            "option_main", "option_vibration", "option_delete"};

const int sModeMessageIds[5] = {0x60003, 0x60004, 0x60005, 0x60006, 0x60007};
const int sLevelMessageIds[3][5] = {
    {0, 0x6000C, 0x6000D, 0x6000E, 0x6000F},
    {0, 0x60010, 0x60011, 0x60012, 0x60013},
    {0, 0x60002, 0, 0, 0},
};

const TitleText sTitleTexts[35] = {
    {SCREEN_TITLE, "TEXT_01", 0x600D4, L"はじめから"},
    {SCREEN_TITLE, "TEXT_02", 0x600D5, L"つづきから"},
    {SCREEN_TITLE, "TEXT_03", 0x600D6, L"オプション"},
    {SCREEN_NEW_GAME, "TEXT_01", 0x600DD, L"セーブデータ A"},
    {SCREEN_NEW_GAME, "TEXT_05", 0x600DD, L"セーブデータ A"},
    {SCREEN_NEW_GAME, "TEXT_02", 0x600DE, L"セーブデータ B"},
    {SCREEN_NEW_GAME, "TEXT_04", 0x600DE, L"セーブデータ B"},
    {SCREEN_NEW_GAME, "TEXT_00", 0x600DF, L"セーブデータ C"},
    {SCREEN_NEW_GAME, "TEXT_03", 0x600DF, L"セーブデータ C"},
    {SCREEN_CONTINUE, "TEXT_01", 0x600DD, L"セーブデータ A"},
    {SCREEN_CONTINUE, "TEXT_05", 0x600DD, L"セーブデータ A"},
    {SCREEN_CONTINUE, "TEXT_02", 0x600DE, L"セーブデータ B"},
    {SCREEN_CONTINUE, "TEXT_04", 0x600DE, L"セーブデータ B"},
    {SCREEN_CONTINUE, "TEXT_00", 0x600DF, L"セーブデータ C"},
    {SCREEN_CONTINUE, "TEXT_03", 0x600DF, L"セーブデータ C"},
    {SCREEN_DIFFICULTY, "title", 0x600D5, L"つづきから"},
    {SCREEN_DIFFICULTY, "title_00", 0x600DC, L"モード"},
    {SCREEN_DIFFICULTY, "TEXT_01", 0x60000, L"ノーマル"},
    {SCREEN_DIFFICULTY, "TEXT_02", 0x60001, L"アナザー"},
    {SCREEN_DIFFICULTY, "TEXT_03", 0x60002, L"エクストラ"},
    {SCREEN_OPTIONS, "title_00", 0x600D6, L"オプション"},
    {SCREEN_OPTIONS, "TEXT_01", 0x600D7, L"しんどうの せってい"},
    {SCREEN_OPTIONS, "TEXT_02", 0x600DB, L"セーブデータの さくじょ"},
    {SCREEN_VIBRATION, "title", 0x600D6, L"オプション"},
    {SCREEN_VIBRATION, "title_00", 0x600D8, L"ゲームキューブコントローラの\nしんどう"},
    {SCREEN_VIBRATION, "TEXT_01", 0x600D9, L"ON"},
    {SCREEN_VIBRATION, "TEXT_02", 0x600DA, L"OFF"},
    {SCREEN_DELETE, "title", 0x600D6, L"オプション"},
    {SCREEN_DELETE, "title_00", 0x600DB, L"セーブデータの さくじょ"},
    {SCREEN_DELETE, "TEXT_01", 0x600DD, L"セーブデータ A"},
    {SCREEN_DELETE, "TEXT_05", 0x600DD, L"セーブデータ A"},
    {SCREEN_DELETE, "TEXT_02", 0x600DE, L"セーブデータ B"},
    {SCREEN_DELETE, "TEXT_04", 0x600DE, L"セーブデータ B"},
    {SCREEN_DELETE, "TEXT_00", 0x600DF, L"セーブデータ C"},
    {SCREEN_DELETE, "TEXT_03", 0x600DF, L"セーブデータ C"},
};

class CTitleSession : public ITitleSession {
public:
    CTitleSession() : mUIManager(new CUIManager), mMsgWin(IDlgMsgWin::Create(false, true)) {}
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual void Run();

private:
    bool RefreshSlots();
    void WaitForMacAddress();
    void SaveAndWait();
    bool IsFinished();

    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mLayouts[SCREEN_COUNT];
    std::tr1::shared_ptr<IDlgMsgWin> mMsgWin;
    std::tr1::shared_ptr<ISaveManager> mSaveManager;
    std::wstring mSlotTexts[SLOT_COUNT];
    bool mButtonEnabled[SCREEN_COUNT][BUTTON_COUNT];
    int mSlotsInUse;
    int mLastPlayedSlot;
    int mFirstEmptySlot;
    int mFirstUsedSlot;
};

}

std::tr1::shared_ptr<ITitleSession> ITitleSession::Create() {
    return std::tr1::shared_ptr<ITitleSession>(new CTitleSession);
}

namespace {

void CTitleSession::Draw() {
    mUIManager->Draw();
    mMsgWin->Draw();
}

void CTitleSession::CalcView() {
    mUIManager->CalcView();
    mMsgWin->CalcView();
}

void CTitleSession::CalcMtx() {
    mUIManager->CalcMtx();
    mMsgWin->CalcMtx();
}

void CTitleSession::Calc() {
    mUIManager->Calc();
    mMsgWin->Calc();
}

#pragma dont_inline on
void CTitleSession::Run() {
    const char** pArchives;
    std::tr1::shared_ptr<FileList> files = LoadFileList(reinterpret_cast<void**>(&pArchives), "title2d.arc", 0, true, nullptr);
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    for (int i = 0; i < SCREEN_COUNT; ++i) {
        mLayouts[i].reset(new BUILayout);
        mLayouts[i]->Load(pArchives, sScreenLayouts[i], fonts, true);
        mLayouts[i]->GetLayout()->SetVisible(false);
        mUIManager->AddChild(mLayouts[i]);
    }
    for (int i = 0; i < 35; ++i) {
        mLayouts[sTitleTexts[i].mScreen]->GetLayout()->SetText(sTitleTexts[i].mPane, GetMessageText(sTitleTexts[i].mMessageId, sTitleTexts[i].mDefault));
    }

    YieldFiberStarted();
    std::vector<IController*> controllers;
    controllers.push_back(GetController(IController::TYPE_WII_REMOTE, 0));
    GetControllerTable()->SetControllers(controllers);

    int selection = RefreshSlots();
    CLayout* pLayout = nullptr;
    int screen = SCREEN_TITLE;
    int shownScreen = SCREEN_NONE;
    int shownSelection = -1;
    ChangeBGM(BGM_TITLE);
    while (!IsFinished()) {
        if ((selection != shownSelection || screen != shownScreen) && shownSelection != -1) {
            pLayout->CreateMotion("hover_normal", sButtonPanes[shownSelection], 0.0f, 1.0f);
        }
        std::tr1::shared_ptr<CLayout::CMotionProgram> motion;
        if (screen != shownScreen) {
            if (shownScreen != SCREEN_NONE) {
                if (shownScreen != SCREEN_TITLE) {
                    PlaySound(41);
                }
                pLayout->CreateMotion("end")->Wait();
                pLayout->SetVisible(false);
            }
            shownScreen = screen;
            if (screen != SCREEN_TITLE) {
                PlaySound(40);
            }
            pLayout = mLayouts[screen]->GetLayout();
            pLayout->SetVisible(true);
            motion = pLayout->CreateMotion("begin");
            switch (screen) {
            case SCREEN_TITLE:
                pLayout->QueueMotion("title_loop", "TITLE", 0.0f, 1.0f);
                break;
            case SCREEN_NEW_GAME:
                pLayout->SetText("title", GetMessageString(0x600D4, L"はじめから"));
                break;
            case SCREEN_CONTINUE:
                pLayout->SetText("title", GetMessageString(0x600D5, L"つづきから"));
                break;
            case SCREEN_VIBRATION:
                pLayout->SetPaneVisible(GetOption() ? "off" : "on", true);
                pLayout->SetPaneVisible(GetOption() ? "on" : "off", false);
                break;
            }
            shownSelection = -1;
        }
        if (selection != shownSelection) {
            shownSelection = selection;
            pLayout->QueueMotion("normal_hover", sButtonPanes[selection], 0.0f, 1.0f)->Wait();
            pLayout->AppendMotion("hover", sButtonPanes[selection], 0.0f, 1.0f);
        }
        if (motion) {
            motion->Wait();
        }

        IUIInput* pInput = GetUIInput();
        bool connected = false;
        if (pInput != nullptr && pInput->IsConnected()) {
            connected = true;
        }
        u32 trig;
        if (connected) {
            trig = pInput->GetTrig();
        } else {
            trig = 0;
        }

        int step = 0;
        if (shownScreen != SCREEN_VIBRATION) {
            if (trig & INPUT_UP) {
                step = 3;
            }
            if (trig & INPUT_DOWN) {
                step = 1;
            }
        } else if ((trig & INPUT_LEFT) || (trig & INPUT_RIGHT)) {
            switch (shownSelection) {
            case 0:
                selection = 1;
                break;
            case 1:
                selection = 0;
                break;
            }
        } else if ((trig & INPUT_UP) || (trig & INPUT_DOWN)) {
            if (shownSelection != BUTTON_BACK) {
                selection = BUTTON_BACK;
            } else {
                selection = GetOption() != 0;
            }
        }
        if (step != 0) {
            do {
                selection = (selection + step) % BUTTON_COUNT;
            } while (!mButtonEnabled[shownScreen][selection]);
        }
        if (shownSelection != selection) {
            PlaySound(84);
        }
        if ((trig & INPUT_CANCEL) && shownScreen != SCREEN_TITLE) {
            PlaySound(90);
            switch (shownScreen) {
            case SCREEN_NEW_GAME:
                screen = SCREEN_TITLE;
                selection = 0;
                break;
            case SCREEN_CONTINUE:
                screen = SCREEN_TITLE;
                selection = 1;
                break;
            case SCREEN_DIFFICULTY:
                selection = mLastPlayedSlot;
                screen = SCREEN_CONTINUE;
                break;
            case SCREEN_OPTIONS:
                screen = SCREEN_TITLE;
                selection = 2;
                break;
            case SCREEN_VIBRATION:
                screen = SCREEN_OPTIONS;
                selection = 0;
                break;
            case SCREEN_DELETE:
                screen = SCREEN_OPTIONS;
                selection = 1;
                break;
            }
        } else if (trig & INPUT_DECIDE) {
            if (shownSelection != BUTTON_BACK) {
                PlaySound(9);
            } else {
                PlaySound(90);
            }
            pLayout->CreateMotion("pressed", sButtonPanes[shownSelection], 0.0f, 1.0f)->Wait();
            switch (shownScreen) {
            case SCREEN_TITLE:
                switch (shownSelection) {
                case 0:
                    selection = mFirstEmptySlot;
                    screen = SCREEN_NEW_GAME;
                    break;
                case 1:
                    selection = mLastPlayedSlot;
                    screen = SCREEN_CONTINUE;
                    break;
                case 2:
                    screen = SCREEN_OPTIONS;
                    selection = 0;
                    break;
                }
                break;
            case SCREEN_NEW_GAME:
                if (shownSelection < SLOT_COUNT) {
                    SetCurrentSlot(shownSelection);
                    WaitForMacAddress();
                    return;
                }
                screen = SCREEN_TITLE;
                selection = 0;
                break;
            case SCREEN_CONTINUE:
                if (shownSelection < SLOT_COUNT) {
                    mLastPlayedSlot = shownSelection;
                    int unlocked = GetSaveData(shownSelection)->mData.mFlags[1];
                    if (unlocked > 0) {
                        screen = SCREEN_DIFFICULTY;
                        selection = GetSaveData(shownSelection)->mData.mFlags[3];
                        for (int i = 0; i < SLOT_COUNT; ++i) {
                            mButtonEnabled[SCREEN_DIFFICULTY][i] = i <= unlocked;
                            mLayouts[SCREEN_DIFFICULTY]->GetLayout()->SetPaneVisible(sButtonPanes[i], mButtonEnabled[SCREEN_DIFFICULTY][i]);
                        }
                        break;
                    }
                    shownSelection = 0;
                    goto startGame;
                } else {
                    screen = SCREEN_TITLE;
                    selection = 1;
                    break;
                }
            case SCREEN_DIFFICULTY:
                if (shownSelection >= SLOT_COUNT) {
                    selection = mLastPlayedSlot;
                    screen = SCREEN_CONTINUE;
                    break;
                }
            startGame:
                SetCurrentSlot(mLastPlayedSlot);
                if (shownSelection != GetSaveData(-1)->mData.mFlags[3]) {
                    s8 level = 1;
                    switch (shownSelection) {
                    case 0:
                        level = GetSaveData(-1)->mData.mLastArea[0];
                        break;
                    case 1:
                        level = GetSaveData(-1)->mData.mLastArea[1];
                        break;
                    }
                    if (level == 0) {
                        level = 1;
                    }
                    GetSaveData(-1)->mData.mFlags[2] = level;
                    GetSaveData(-1)->mData.mFlags[3] = shownSelection;
                }
                return;
            case SCREEN_OPTIONS:
                switch (shownSelection) {
                case 0:
                    screen = SCREEN_VIBRATION;
                    selection = GetOption() != 0;
                    break;
                case 1:
                    selection = mFirstUsedSlot;
                    screen = SCREEN_DELETE;
                    break;
                default:
                    screen = SCREEN_TITLE;
                    selection = 2;
                    break;
                }
                break;
            case SCREEN_VIBRATION:
                if (static_cast<u32>(shownSelection) <= 1) {
                    if (shownSelection != GetOption()) {
                        SetOption(shownSelection == 1);
                        pLayout->SetPaneVisible(GetOption() ? "off" : "on", true);
                        pLayout->SetPaneVisible(GetOption() ? "on" : "off", false);
                        SaveAndWait();
                    }
                } else {
                    screen = SCREEN_OPTIONS;
                    selection = 0;
                }
                break;
            case SCREEN_DELETE:
                if (shownSelection < SLOT_COUNT) {
                    mMsgWin->SetDefaultYes(false);
                    if (mMsgWin->AskYesNo(FormatMessage(GetMessageString(0x600E0, L"{1}を さくじょします
よろしいですか？"),
                                                        GetMessageString(0x600DD + shownSelection, L"セーブデータ A"))
                                              .c_str())) {
                        mMsgWin->SetDefaultYes(false);
                        if (mMsgWin->AskYesNo(GetMessageString(0x600E1, L"さくじょした データは もとに もどりません\nほんとうに さくじょしますか？"))) {
                            ResetSlot(shownSelection);
                            SaveAndWait();
                            RefreshSlots();
                            if (mSlotsInUse == 0) {
                                screen = SCREEN_OPTIONS;
                                selection = 0;
                            } else {
                                selection = mFirstUsedSlot;
                            }
                        }
                    }
                } else {
                    screen = SCREEN_OPTIONS;
                    selection = 1;
                }
                break;
            }
        }
        YieldFiber();
    }
}

#pragma dont_inline reset
bool CTitleSession::RefreshSlots() {
    mButtonEnabled[0][0] = true;
    mButtonEnabled[0][1] = true;
    mButtonEnabled[0][2] = true;
    mButtonEnabled[0][3] = false;
    for (int i = SCREEN_NEW_GAME; i < SCREEN_COUNT; ++i) {
        mButtonEnabled[i][0] = true;
        mButtonEnabled[i][1] = true;
        mButtonEnabled[i][2] = false;
        mButtonEnabled[i][3] = true;
    }
    mSlotsInUse = 0;
    mLastPlayedSlot = -1;
    mFirstEmptySlot = -1;
    mFirstUsedSlot = -1;


    s64 latest = 0;
    for (int i = 0; i < SLOT_COUNT; ++i) {
        bool used = GetSaveData(i)->mData.mTime > 0;
        std::wstring text(GetMessageString(0x600D2, nullptr));
        if (used) {
            if (mFirstUsedSlot < 0) {
                mFirstUsedSlot = i;
            }
            s64 saved = static_cast<s64>(OS_TIME_SPEED) * GetSaveData(i)->mData.mElapsedSeconds;
            if (latest < saved) {
                mLastPlayedSlot = i;
                latest = saved;
            }
            OSCalendarTime calendar;
            OSTicksToCalendarTime(saved, &calendar);
            std::wstring date;
            FormatDate(date, calendar.year, calendar.mon + 1, calendar.mday, 0);
            std::wstring time;
            FormatTime(time, calendar.hour, calendar.min, calendar.sec, 8);
            text.assign(date + L" " + time);
            int level = GetSaveData(i)->mData.mFlags[3];
            int mode = GetSaveData(i)->mData.mFlags[2];
            if (GetSaveData(i)->mData.mFlags[1] > 0) {
                text.assign(text + L" " + GetMessageString(sLevelMessageIds[level][mode], nullptr));
            } else {
                text.assign(text + L" " + GetMessageString(sModeMessageIds[mode], nullptr));
            }
            u16 monsno = GetSaveData(i)->mData.mLeadPiiMonsno;
            if (monsno != 0) {
                text.assign(text + L" " + GetMessageString(monsno + 0x30000 - 1, nullptr));
            }
            ++mSlotsInUse;
        } else if (mFirstEmptySlot < 0) {
            mFirstEmptySlot = i;
        }
        mLayouts[SCREEN_NEW_GAME]->GetLayout()->SetText(sSlotTextPanes[i], text.c_str());
        mLayouts[SCREEN_CONTINUE]->GetLayout()->SetText(sSlotTextPanes[i], text.c_str());
        mLayouts[SCREEN_DELETE]->GetLayout()->SetText(sSlotTextPanes[i], text.c_str());
        mButtonEnabled[SCREEN_NEW_GAME][i] = !used;
        mButtonEnabled[SCREEN_CONTINUE][i] = used;
        mButtonEnabled[SCREEN_DELETE][i] = used;
        mSlotTexts[i].assign(text);
    }

    bool hasSlots = true;
    if (mSlotsInUse == 0) {
        mButtonEnabled[SCREEN_TITLE][1] = false;
        hasSlots = false;
        mButtonEnabled[SCREEN_OPTIONS][1] = false;
    } else if (mSlotsInUse == SLOT_COUNT) {
        mButtonEnabled[SCREEN_TITLE][0] = false;
    }
    for (int i = 0; i < BUTTON_COUNT; ++i) {
        mLayouts[SCREEN_TITLE]->GetLayout()->SetPaneVisible(sButtonPanes[i], mButtonEnabled[SCREEN_TITLE][i]);
        mLayouts[SCREEN_OPTIONS]->GetLayout()->SetPaneVisible(sButtonPanes[i], mButtonEnabled[SCREEN_OPTIONS][i]);
    }
    return hasSlots;
}

void CTitleSession::WaitForMacAddress() {
    while (true) {
        s32 result = NETGetWirelessMacAddress(GetSaveData(-1)->mData.mWirelessMacAddress);
        if (result == 0) {
            return;
        }
        if (result != -8 && result != -4) {
            return;
        }
        YieldFiber();
    }
}

void CTitleSession::SaveAndWait() {
    if (!mSaveManager) {
        mSaveManager = ISaveManager::Create();
    }
    mSaveManager->RequestSave();
    while (!mSaveManager->IsIdle()) {
        YieldFiber();
    }
}

bool CTitleSession::IsFinished() {
    return false;
}

}
