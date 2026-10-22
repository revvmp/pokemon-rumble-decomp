#include <game/IDlgMsgWin.h>

#include <game/CColorBtnTagProc.h>
#include <game/CLayout.h>
#include <game/ICalcDraw.h>
#include <game/IController.h>
#include <game/IGameMap.h>
#include <game/IGameSession.h>
#include <game/CPlayer.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/Sound.h>
#include <memory>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <string>
#include <vector>

IUIInput* GetUIInput();

#pragma dont_inline on

class CDlgMsgWin : public IDlgMsgWin, public std::tr1::enable_shared_from_this<CDlgMsgWin> {
public:
    enum Mode {
        MODE_MESSAGE,
        MODE_YES_NO,
        MODE_OPTIONS,
        MODE_WAIT
    };

    struct Option {
        std::wstring mLabel;
        std::wstring mText;
    };

    explicit CDlgMsgWin(bool registerForUpdates)
        : mRegisterForUpdates(registerForUpdates) {}
    virtual void SetControllerSlot(int slot) {
        mControllerSlot = slot;
    }

    virtual void OpenMessage(const wchar_t* pText) {
        mMode = MODE_MESSAGE;
        mOpen = true;
        RegisterDialog();
        mLayout.SetPaneVisible("TEXT_ONLY", true);
        mLayout.SetPaneVisible("WITH_IMAGE", false);
        mLayout.SetPaneVisible("PROMPT", true);
        mLayout.SetPaneVisible("YESNO", false);
        mLayout.SetPaneVisible("3ITEMS", false);
        mLayout.SetPaneVisible("4ITEMS", false);
        mLayout.SetPaneVisible("FOOTNOTE", false);
        CColorBtnTagProc::Attach(mLayout.SetText("MESSAGE", pText));
        mMotion = mLayout.CreateMotion("begin");
        mLayout.CreateMotion("loop", mLayout.FindPane("PROMPT"), 0.0f, 1.0f);
        mLayout.CreateMotion("loop", mLayout.FindPane("YES"), 0.0f, 1.0f);
        mLayout.CreateMotion("loop", mLayout.FindPane("NO"), 0.0f, 1.0f);
    }

    virtual void OpenImageMessage(const wchar_t* pText, const char* pImage) {
        mMode = MODE_MESSAGE;
        mOpen = true;
        RegisterDialog();
        mLayout.SetPaneVisible("TEXT_ONLY", false);
        mLayout.SetPaneVisible("WITH_IMAGE", true);
        mLayout.SetPaneVisible("PROMPT", true);
        mLayout.SetPaneVisible("YESNO", false);
        mLayout.SetPaneVisible("3ITEMS", false);
        mLayout.SetPaneVisible("4ITEMS", false);
        mLayout.SetPaneVisible("FOOTNOTE", false);
        CColorBtnTagProc::Attach(mLayout.SetText("W_IMG_MSG", pText));
        mLayout.SetImage("IMAGE", pImage, -1);
        mMotion = mLayout.CreateMotion("begin");
        mLayout.CreateMotion("loop", mLayout.FindPane("PROMPT"), 0.0f, 1.0f);
        mLayout.CreateMotion("loop", mLayout.FindPane("YES"), 0.0f, 1.0f);
        mLayout.CreateMotion("loop", mLayout.FindPane("NO"), 0.0f, 1.0f);
    }

    virtual bool IsMessageClosed() {
        return !mOpen;
    }

    virtual bool IsAnimationDone() {
        bool done = false;
        if (!mOpen && (!mMotion || mMotion->IsEnded(false))) {
            done = true;
        }
        return done;
    }

    virtual void SetDefaultYes(bool yes) {
        mDefaultOption = !yes;
    }

    virtual void OpenYesNo(const wchar_t* pText) {
        mMode = MODE_YES_NO;
        if (mDefaultOption != 0 && mDefaultOption != 1) {
            mDefaultOption = 0;
        }
        mCursor = mDefaultOption;
        mOpen = true;
        RegisterDialog();
        mLayout.SetPaneVisible("TEXT_ONLY", true);
        mLayout.SetPaneVisible("WITH_IMAGE", false);
        mLayout.SetPaneVisible("PROMPT", false);
        mLayout.SetPaneVisible("YES_CURSOR", mCursor == 0);
        mLayout.SetPaneVisible("NO_CURSOR", mCursor == 1);
        mLayout.SetPaneVisible("YESNO", true);
        mLayout.SetPaneVisible("3ITEMS", false);
        mLayout.SetPaneVisible("4ITEMS", false);
        mLayout.SetPaneVisible("FOOTNOTE", false);
        CColorBtnTagProc::Attach(mLayout.SetText("MESSAGE", pText));
        mLayout.SetText("YES", GetMessageString(0x10046, L"はい"));
        mLayout.SetText("NO", GetMessageString(0x10047, L"いいえ"));
        mMotion = mLayout.CreateMotion("begin");
    }

    virtual bool IsYesNoClosed() {
        return !mOpen;
    }

    virtual bool IsYesSelected() {
        return mCursor == 0;
    }

    virtual void SetOption(int index, const wchar_t* pLabel, const wchar_t* pText) {
        if (index >= mOptions.size()) {
            mOptions.resize(index + 1);
        }
        mOptions[index].mLabel = pLabel;
        mOptions[index].mText = pText;
    }

    virtual void SetDefaultOption(int index) {
        mDefaultOption = index;
    }

    virtual void OpenOptions(const wchar_t* pText) {
        mMode = MODE_OPTIONS;
        if (mDefaultOption < 0) {
            mDefaultOption = 0;
        }
        if (mDefaultOption >= mOptions.size()) {
            mDefaultOption = mOptions.size() - 1;
        }
        mCursor = mDefaultOption;
        mOpen = true;
        RegisterDialog();
        mLayout.SetPaneVisible("TEXT_ONLY", true);
        mLayout.SetPaneVisible("WITH_IMAGE", false);
        mLayout.SetPaneVisible("PROMPT", false);
        mLayout.SetPaneVisible("YESNO", false);
        int count = mOptions.size() < 4 ? 3 : 4;
        mLayout.SetPaneVisible("3ITEMS", count == 3);
        mLayout.SetPaneVisible("4ITEMS", count == 4);
        CColorBtnTagProc::Attach(mLayout.SetText("MESSAGE", pText));
        mMotion = mLayout.CreateMotion("begin");
        char itemName[] = "I3_0";
        bool hasText = false;
        for (unsigned int i = 0; i < count; i++) {
            char name[] = "I3_0";
            name[1] = '0' + count;
            name[3] = '0' + i;
            CColorBtnTagProc::Attach(mLayout.SetText(name, mOptions[i].mLabel.c_str()));
            bool visible = i < mOptions.size();
            mLayout.SetPaneVisible(name, visible);
            if (visible && mOptions[i].mText.size() != 0) {
                hasText = true;
            }
        }
        mLayout.SetPaneVisible("FOOTNOTE", hasText);
        CColorBtnTagProc::Attach(mLayout.SetText("FOOTNOTE", mOptions[mCursor].mText.c_str()));
        char cursorName[] = "I3_0_CURSOR";
        for (unsigned int i = 0; i < count; i++) {
            char name[] = "I3_0_CURSOR";
            name[1] = '0' + count;
            name[3] = '0' + i;
            mLayout.SetPaneVisible(name, i == mCursor);
            mLayout.CreateMotion("loop", mLayout.FindPane(name), 0.0f, 1.0f);
        }
    }

    virtual bool IsOptionsClosed() {
        return !mOpen;
    }

    virtual int GetSelectedOption() {
        return mCursor;
    }

    virtual void OpenFootnote(const wchar_t* pText) {
        mMode = MODE_WAIT;
        mOpen = true;
        RegisterDialog();
        mLayout.SetPaneVisible("TEXT_ONLY", true);
        mLayout.SetPaneVisible("WITH_IMAGE", false);
        mLayout.SetPaneVisible("PROMPT", false);
        mLayout.SetPaneVisible("YESNO", false);
        mLayout.SetPaneVisible("3ITEMS", false);
        mLayout.SetPaneVisible("4ITEMS", false);
        mLayout.SetPaneVisible("FOOTNOTE", false);
        CColorBtnTagProc::Attach(mLayout.SetText("MESSAGE", pText));
        mMotion = mLayout.CreateMotion("begin");
    }

    virtual void OpenNotice(const wchar_t* pText) {
        OpenFootnote(pText);
        mLayout.SetPaneVisible("PROMPT", true);
        mLayout.CreateMotion("loop", mLayout.FindPane("PROMPT"), 0.0f, 1.0f);
    }

    virtual bool Close() {
        Dismiss();
        return true;
    }

    virtual bool IsClosed() {
        bool closed = false;
        if (!mOpen && (!mMotion || mMotion->IsEnded(false))) {
            closed = true;
        }
        return closed;
    }

    void Dismiss() {
        mOpen = false;
        mOptions.resize(0);
        mDefaultOption = 0;
        mMotion = mLayout.CreateMotion("off");
        if (mRegisterForUpdates) {
            GetCurrentGameMap()->UnregisterDialog(shared_from_this());
        }
    }

    void RegisterDialog() {
        if (mRegisterForUpdates) {
            GetCurrentGameMap()->RegisterDialog(shared_from_this());
        }
    }

    virtual void Calc() {
        IUIInput* pInput = GetUIInput();
        int slot = mControllerSlot;
        if (slot >= 0) {
            pInput = IGameSession::GetInstance()->GetPlayer(slot)->GetController();
        }
        bool connected = false;
        if (pInput && pInput->IsConnected()) {
            connected = true;
        }
        u32 trig;
        if (connected) {
            trig = pInput->GetTrig();
        } else {
            trig = 0;
        }
        if (mOpen && (!mMotion || mMotion->IsEnded(false))) {
            if (mMode == MODE_YES_NO || mMode == MODE_OPTIONS) {
                int oldCursor = mCursor;
                if ((trig & 0x1) && oldCursor > 0) {
                    mCursor = oldCursor - 1;
                }
                if (trig & 0x4) {
                    int last = mMode == MODE_YES_NO ? 1 : mOptions.size() - 1;
                    if (mCursor < last) {
                        mCursor = mCursor + 1;
                    }
                }
                if (mMode == MODE_OPTIONS && oldCursor != mCursor) {
                    int itemCount = mOptions.size() < 4 ? 3 : 4;
                    char name[] = "I3_0_CURSOR";
                    name[1] = '0' + itemCount;
                    name[3] = '0' + oldCursor;
                    mLayout.SetPaneVisible(name, false);
                    name[3] = '0' + mCursor;
                    mLayout.SetPaneVisible(name, true);
                    CColorBtnTagProc::Attach(mLayout.SetText("FOOTNOTE", mOptions[mCursor].mText.c_str()));
                    mLayout.UpdateAttachedLayouts();
                }
                if (mMode == MODE_YES_NO && oldCursor != mCursor) {
                    mLayout.SetPaneVisible("YES_CURSOR", mCursor == 0);
                    mLayout.SetPaneVisible("NO_CURSOR", mCursor == 1);
                    mLayout.UpdateAttachedLayouts();
                }
                if (oldCursor != mCursor) {
                    PlaySound(0x54);
                }
            }
            if ((trig & 0x20) && mMode != MODE_WAIT) {
                switch (mMode) {
                case MODE_MESSAGE:
                    PlaySound(0x8);
                    break;
                case MODE_YES_NO:
                    if (mCursor != 0) {
                        PlaySound(0x5a);
                    } else {
                        PlaySound(0x9);
                    }
                    break;
                }
                Dismiss();
            }
        }
        mLayout.Calc();
    }

    virtual void Draw() {
        mLayout.Draw();
    }

    void Initialize(bool background) {
        std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
        mLayout.Load(gSharedLayoutArchives, "dlgmsgwin", fonts, background);
        mOptions.reserve(4);
        mDefaultOption = 0;
        mOpen = false;
        mLayout.CreateMotion("off");
        mControllerSlot = -1;
    }

private:
    bool mRegisterForUpdates;
    CLayout mLayout;
    bool mOpen;
    std::vector<Option> mOptions;
    int mDefaultOption;
    int mCursor;
    int mMode;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
    int mControllerSlot;
};

std::tr1::shared_ptr<IDlgMsgWin> IDlgMsgWin::Create(bool registerForUpdates, bool background) {
    std::tr1::shared_ptr<CDlgMsgWin> pDialog(new CDlgMsgWin(registerForUpdates));
    pDialog->Initialize(background);
    return pDialog;
}
