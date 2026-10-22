#include <memory>
#include <game/CFiber.h>
#include <string>
#include <game/INumericInput.h>
#include <game/CUIManager.h>
#include <game/BUILayout.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/IGameMap.h>
#include <game/IGameSession.h>
#include <game/CPlayer.h>
#include <game/IController.h>
#include <game/Sound.h>
#include <game/MathUtil.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <memory>
#include <string>
#include <algorithm>

IUIInput* GetUIInput();

static const int sKeyGrid[5][4] = {
    {2, 3, 4, 11},
    {5, 6, 7, 0},
    {8, 9, 10, 0},
    {0, 1, 0, 0},
    {13, 0, 12, 0}
};

static const char* sKeyPaneNames[] = {
    nullptr, "W_CPkey_10", "W_CPkey_00", "W_CPkey_01", "W_CPkey_02", "W_CPkey_03", "W_CPkey_04", "W_CPkey_05", "W_CPkey_06", "W_CPkey_07", "W_CPkey_08", "W_CPkey_DELETE", "P_BT_confirm", "P_BT_cancel"
};

class CNumericInput : public INumericInput {
public:
    CNumericInput() : mManager(new CUIManager), mBackground(new BUILayout), mKeyboard(new BUILayout), mTextBox(new BUILayout), mConfirmed(false), mStarted(false), mPlayer(-1) {}
    virtual void Calc() { mFiber.Resume(); mManager->Calc(); }
    virtual void Draw() { if (mStarted) mManager->Draw(); }
    virtual void Start(int player) { mPlayer = player; mStarted = true; }
    virtual bool IsFinished() const { return !mStarted; }
    virtual bool IsConfirmed() const { return mConfirmed; }
    virtual std::wstring GetText() const { return mText; }
    virtual void Run();

    void Initialize(bool background);

private:
    void UpdateText() {
        CLayout* pLayout = mTextBox->GetLayout();
        pLayout->SetText("T_2l_TextBox", mText.c_str());
        nw4r::lyt::TextBox* pText = static_cast<nw4r::lyt::TextBox*>(pLayout->FindPane("T_2l_TextBox"));
        nw4r::ut::Rect rect;
        if (mText.size()) rect = pText->GetTextDrawRect();
        else rect.right = pText->GetPaneRect().left;
        nw4r::lyt::Pane* pCursor = pLayout->FindPane("Cursor");
        nw4r::math::VEC3 position = pCursor->GetTranslate();
        position.x = rect.right;
        pCursor->SetTranslate(position);
    }

    std::tr1::shared_ptr<CUIManager> mManager;
    std::tr1::shared_ptr<BUILayout> mBackground;
    std::tr1::shared_ptr<BUILayout> mKeyboard;
    std::tr1::shared_ptr<BUILayout> mTextBox;
    std::wstring mText;
    CFiber mFiber;
    bool mConfirmed;
    bool mStarted;
    int mPlayer;
};

inline void CNumericInput::Run() {
    YieldFiberStarted();
    for (;;) {
waitStart:
        while (!mStarted) YieldFiber();
        mConfirmed = false;
        mText.clear();
        IGameMap* pMap = GetCurrentGameMap();
        if (pMap) pMap->SetPausing(true);
        int selected = 0;
        int key = 2;
        int column;
        int row;
        for (row = 0; row < 5; ++row) {
            for (column = 0; column < 4; ++column) {
                if (sKeyGrid[row][column] == key) goto foundInitial;
            }
        }
foundInitial:
        UpdateText();
        mTextBox->GetLayout()->CreateMotion("blink", "Cursor", 0.0f, 1.0f);
        for (;;) {
            if (selected != key) {
                if (selected) {
                    mKeyboard->GetLayout()->AppendMotion("Focus-OUT", sKeyPaneNames[selected], 0.0f, 1.0f);
                    PlaySound(84);
                }
                selected = key;
                if (selected) mKeyboard->GetLayout()->AppendMotion("Focus-IN", sKeyPaneNames[selected], 0.0f, 1.0f);
            }
            IUIInput* pInput = GetUIInput();
            if (mPlayer >= 0) pInput = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
            bool connected = pInput && pInput->IsConnected();
            u32 repeat = connected ? pInput->GetTrig() : 0;
            if (repeat & 15) {
                int direction = 0;
                if (repeat & 8) direction = 3;
                if (repeat & 2) direction = 1;
                if (repeat & 1) --row;
                if (repeat & 4) ++row;
                row = Clamp(row, 0, 4);
                for (;;) {
                    column = (column + direction) % 4;
                    key = sKeyGrid[row][column];
                    if (key) break;
                    if (!direction) {
                        direction = -1;
                        if (selected != 11) direction = 1;
                    }
                }
            }
            if (repeat & 16) {
                PlaySound(90);
                if (mText.size()) {
                    mText.resize(mText.size() - 1);
                    UpdateText();
                } else {
                    key = 13;
                    for (row = 0; row < 5; ++row) {
                        for (column = 0; column < 4; ++column) {
                            if (sKeyGrid[row][column] == key) goto foundCancel;
                        }
                    }
                }
foundCancel:
                mKeyboard->GetLayout()->CreateMotion("Pushed", sKeyPaneNames[11], 0.0f, 1.0f);
                if (selected != 11) mKeyboard->GetLayout()->QueueMotion("Focus-OUT", sKeyPaneNames[11], 0.0f, 1.0f);
            } else if (repeat & 32) {
                switch (selected) {
                case 12:
                case 13:
                {
                    mConfirmed = selected == 12 && mText.size() != 0;
                    if (mConfirmed) PlaySound(9);
                    else PlaySound(90);
                    goto finish;
                }
                case 11:
                {
                    PlaySound(90);
                    if (mText.size()) mText.resize(mText.size() - 1);
                    break;
                }
                default:
                {
                    if (mText.size() < 8) {
                        PlaySound(9);
                        mText += static_cast<wchar_t>(selected + 47);
                        if (mText.size() == 8) {
                            key = 12;
                            for (row = 0; row < 5; ++row) {
                                for (column = 0; column < 4; ++column) {
                                    if (sKeyGrid[row][column] == key) goto foundConfirm;
                                }
                            }
                        }
                    } else PlaySound(90);
                    break;
                }
                }
foundConfirm:
                UpdateText();
                mKeyboard->GetLayout()->CreateMotion("Pushed", sKeyPaneNames[selected], 0.0f, 1.0f);
            } else if (repeat & 128) {
                key = 13;
                for (row = 0; row < 5; ++row) {
                    for (column = 0; column < 4; ++column) {
                        if (sKeyGrid[row][column] == key) goto foundBack;
                    }
                }
            }
foundBack:
            YieldFiber();
        }
finish:
        mKeyboard->GetLayout()->CreateMotion("Focus-OUT", sKeyPaneNames[selected], 0.0f, 1.0f)->Wait();
        mStarted = false;
        if (pMap) pMap->SetPausing(true);
    }
}

inline void CNumericInput::Initialize(bool background) {
    const char* const* pArchives = gSharedLayoutArchives;
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    mBackground->Load(pArchives, "fs_VK_bg_a", fonts, background);
    mManager->AddChild(mBackground);
    mKeyboard->Load(pArchives, "fs_VK_cellPhone_a", fonts, background);
    mKeyboard->SetText("T_BT_confirm", GetMessageString(393457, nullptr));
    mKeyboard->SetText("T_BT_cancel", GetMessageString(393458, nullptr));
    mKeyboard->GetLayout()->SetDepthCompare(GX_LEQUAL);
    mManager->AddChild(mKeyboard);
    mTextBox->Load(pArchives, "fs_VK_textBox_b", fonts, background);
    mManager->AddChild(mTextBox);
    mFiber.Start(std::tr1::bind(&CNumericInput::Run, this), 0x8000);
}

std::tr1::shared_ptr<INumericInput> INumericInput::Create(bool background) {
    std::tr1::shared_ptr<CNumericInput> input(new CNumericInput);
    input->Initialize(background);
    return input;
}
