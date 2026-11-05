#include <game/ui/IIconCollection.h>
#include <game/ui/CLayout.h>
#include <game/input/CControllerWiiRemote.h>
#include <game/input/ControllerTable.h>
#include <game/ui/LayoutResources.h>
#include <nw4r/lyt/lyt_pane.h>
#include <memory>
#include <game/ui/CIconCollection.h>

namespace {
const char* sButtonNames[5][2] = {
    {"BTN_GCPAD_A", "BTN_GCPAD_B"},
    {"BTN_WIIREMOTE_2", "BTN_WIIREMOTE_1"},
    {"BTN_CLASSIC_a", "BTN_CLASSIC_b"},
    {"BTN_NUNCHAKU_Z", "BTN_NUNCHAKU_C"},
    {"BTN_WIIREMOTE_2", "BTN_WIIREMOTE_1"},
};
const char* sControllerNames[5][4] = {
    {"CT_GCPAD1", "CT_GCPAD2", "CT_GCPAD3", "CT_GCPAD4"},
    {"CT_WIIREMOTE1", "CT_WIIREMOTE2", "CT_WIIREMOTE3", "CT_WIIREMOTE4"},
    {"CT_CLASSIC1", "CT_CLASSIC2", "CT_CLASSIC3", "CT_CLASSIC4"},
    {"CT_NUNCHAKU1", "CT_NUNCHAKU2", "CT_NUNCHAKU3", "CT_NUNCHAKU4"},
    {"CT_WIIREMOTE1", "CT_WIIREMOTE2", "CT_WIIREMOTE3", "CT_WIIREMOTE4"},
};

inline nw4r::lyt::TexMap CIconCollection::GetControllerIcon(IController::Type type, int channel) {
        if (const char* pName = sControllerNames[type][channel]) {
            if (nw4r::lyt::Pane* pPane = mControllers.FindPane(pName)) {
                const nw4r::lyt::Material* pMaterial = pPane->GetMaterial();
                return pMaterial->GetTexMapAry()[0];
            }
        }
        return nw4r::lyt::TexMap();
    }

inline nw4r::lyt::TexMap CIconCollection::GetControllerIcon(const IController* pController) {
        IController::Type type = pController->GetType();
        if (!IsExtensionControllerEnabled() && type == IController::TYPE_WII_REMOTE) {
            if (const CControllerWiiRemote* pRemote = dynamic_cast<const CControllerWiiRemote*>(pController)) {
                if (pRemote->UsesClassic()) type = IController::TYPE_CLASSIC;
            }
        }
        return GetControllerIcon(type, pController->GetChannel());
    }

inline nw4r::lyt::TexMap CIconCollection::GetButtonIcon(IController::Type type, u32 button) {
        const char* pName = sButtonNames[type][button == 32 ? 0 : 1];
        if (pName != nullptr) {
            if (nw4r::lyt::Pane* pPane = mButtons.FindPane(pName)) {
                const nw4r::lyt::Material* pMaterial = pPane->GetMaterial();
                return pMaterial->GetTexMapAry()[0];
            }
        }
        return nw4r::lyt::TexMap();
    }

inline nw4r::lyt::TexMap CIconCollection::GetButtonIcon(const IController* pController, u32 button) {
        IController::Type type = pController->GetType();
        if (!IsExtensionControllerEnabled() && type == IController::TYPE_WII_REMOTE) {
            if (const CControllerWiiRemote* pRemote = dynamic_cast<const CControllerWiiRemote*>(pController)) {
                if (pRemote->UsesClassic()) type = IController::TYPE_CLASSIC;
            }
        }
        return GetButtonIcon(type, button);
    }

inline void CIconCollection::Initialize(bool background) {
        std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
        mControllers.Load(gSharedLayoutArchives, "icons_ct", fonts, background);
        mButtons.Load(gSharedLayoutArchives, "icons_button", fonts, background);
    }
std::tr1::shared_ptr<CIconCollection> sIconCollection;
}
void InitIconCollection(bool background) {
    sIconCollection.reset(new CIconCollection);
    sIconCollection->Initialize(background);
}
IIconCollection* GetIconCollection() { return sIconCollection.get(); }
void ShutdownIconCollection() { sIconCollection.reset(); }
