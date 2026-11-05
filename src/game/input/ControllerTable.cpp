#include <game/input/CControllerWiiRemote.h>
#include <game/input/CControllerNunchaku.h>
#include <game/input/CControllerClassic.h>
#include <game/input/CControllerGCPad.h>
#include <game/input/CControllerComposition.h>
#include <game/input/ControllerTable.h>

namespace {
CControllerWiiRemote sWiiRemotes[4];
CControllerNunchaku sNunchakus[4];
CControllerClassic sClassics[4];
CControllerGCPad sGCPads[4];
CControllerComposition sUIInput;
BController* sControllers[4][4] = {
    {&sGCPads[0], &sGCPads[1], &sGCPads[2], &sGCPads[3]},
    {&sWiiRemotes[0], &sWiiRemotes[1], &sWiiRemotes[2], &sWiiRemotes[3]},
    {&sClassics[0], &sClassics[1], &sClassics[2], &sClassics[3]},
    {&sNunchakus[0], &sNunchakus[1], &sNunchakus[2], &sNunchakus[3]}
};
bool sExtensionControllerEnabled = true;
}

void InitControllers() {
    for (int type = 0; type < 4; ++type) {
        for (int channel = 0; channel < 4; ++channel) {
            sControllers[type][channel]->SetChannel(channel);
        }
    }
}

void SetExtensionControllerEnabled(bool enabled) { sExtensionControllerEnabled = enabled; }
bool IsExtensionControllerEnabled() { return sExtensionControllerEnabled; }

void UpdateControllers() {
    for (int type = 0; type < 4; ++type) {
        for (int channel = 0; channel < 4; ++channel) {
            sControllers[type][channel]->Update();
        }
    }
    sUIInput.Update();
}

void ShutdownControllers() {}
IController* GetController(IController::Type type, int channel) { return sControllers[type][channel]; }
IUIInput* GetUIInput() { return &sUIInput; }
CControllerComposition* GetControllerTable() { return &sUIInput; }
