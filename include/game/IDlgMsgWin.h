#ifndef GAME_IDLGMSGWIN_H
#define GAME_IDLGMSGWIN_H

#include <game/CFiber.h>
#include <game/IDlg.h>
#include <memory>

class IDlgMsgWin : public IDlg {
public:
    virtual ~IDlgMsgWin() {}
    virtual void SetControllerSlot(int slot) = 0;
    virtual void OpenMessage(const wchar_t* pText) = 0;
    virtual void OpenImageMessage(const wchar_t* pText, const char* pImageName) = 0;
    virtual bool IsMessageClosed() = 0;
    virtual bool IsAnimationDone() = 0;
    virtual void SetDefaultYes(bool yes) = 0;
    virtual void OpenYesNo(const wchar_t* pText) = 0;
    virtual bool IsYesNoClosed() = 0;
    virtual bool IsYesSelected() = 0;
    virtual void SetOption(int index, const wchar_t* pLabel, const wchar_t* pText) = 0;
    virtual void SetDefaultOption(int index) = 0;
    virtual void OpenOptions(const wchar_t* pText) = 0;
    virtual bool IsOptionsClosed() = 0;
    virtual int GetSelectedOption() = 0;
    virtual void OpenFootnote(const wchar_t* pText) = 0;
    virtual void BeginClose() = 0;
    virtual bool IsClosed() = 0;

    bool AskYesNo(const wchar_t* pText) {
        OpenYesNo(pText);
        while (!IsYesNoClosed()) {
            YieldFiber();
        }
        return IsYesSelected();
    }

    void WaitUntilClosed() {
        while (!IsClosed()) {
            YieldFiber();
        }
    }

    void ShowMessage(const wchar_t* pText) {
        OpenMessage(pText);
        while (!IsMessageClosed()) {
            YieldFiber();
        }
    }

    static std::tr1::shared_ptr<IDlgMsgWin> Create(bool registerForUpdates, bool background);
};

#endif
