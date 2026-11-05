#include <game/ui/CUIMenuItem.h>

#include <game/ui/CUIMenu.h>
#include <game/audio/Sound.h>
#include <functional>

namespace {

enum MenuItemKindFlag {
    KIND_DISABLED = 1 << 29,
    KIND_SILENT = 1 << 30
};

enum MenuSoundId {
    SOUND_ACCEPT = 9,
    SOUND_CURSOR = 84,
    SOUND_CANCEL = 90
};

}

CUIMenuSoundCallback::CUIMenuSoundCallback(int mode) : mMode(mode), mMuteSelection(true) {
    mAcceptCallbacks.Add(std::tr1::bind(&CUIMenuSoundCallback::OnAccept, this, std::tr1::placeholders::_1));
    mSelectionCallbacks.Add(std::tr1::bind(&CUIMenuSoundCallback::OnSelectionChange, this, std::tr1::placeholders::_1));
    mScrollCallbacks.Add(std::tr1::bind(&CUIMenuSoundCallback::OnScroll, this, std::tr1::placeholders::_1));
}

void CUIMenuSoundCallback::OnAccept(BUI* pSender) {
    CUIMenu* pMenu = dynamic_cast<CUIMenu*>(pSender);
    if (pMenu != nullptr) {
        std::tr1::shared_ptr<IUIListItem> item = pMenu->GetSelectedItem();
        if (item) {
            std::tr1::shared_ptr<CUIMenuItem> menuItem = std::tr1::dynamic_pointer_cast<CUIMenuItem>(item);
            if (menuItem && !(menuItem->GetKind() & KIND_SILENT)) {
                if (menuItem->GetKind() & KIND_DISABLED) {
                    PlaySound(SOUND_CANCEL);
                } else {
                    PlaySound(SOUND_ACCEPT);
                }
            }
        }
    }
}

void CUIMenuSoundCallback::OnSelectionChange(BUI* pSender) {
    if (mMuteSelection) {
        mMuteSelection = false;
    } else {
        PlaySound(SOUND_CURSOR);
    }
}

void CUIMenuSoundCallback::OnScroll(BUI* pSender) {
    if (mMode != MODE_NO_SCROLL_SOUND) {
        PlaySound(SOUND_CANCEL);
    }
}

inline BUI::Callback BUI::CCallbackListOwner::Add(const std::tr1::function<void(BUI*)>& rCallback) {
    if (rCallback) {
        Callback callback(new std::tr1::function<void(BUI*)>(rCallback));
        if (mpList == nullptr) mpList = new CallbackList();
        mpList->push_back(callback);
        return callback;
    }
    return Callback();
}
