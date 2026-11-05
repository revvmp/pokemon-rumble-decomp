#ifndef GAME_CICONCOLLECTION_H
#define GAME_CICONCOLLECTION_H
#include <game/ui/IIconCollection.h>
#include <game/ui/CLayout.h>

namespace {
class CIconCollection : public IIconCollection {
public:
    virtual nw4r::lyt::TexMap GetControllerIcon(const IController* pController);
    virtual nw4r::lyt::TexMap GetControllerIcon(IController::Type type, int channel);
    virtual nw4r::lyt::TexMap GetButtonIcon(const IController* pController, u32 button);
    virtual nw4r::lyt::TexMap GetButtonIcon(IController::Type type, u32 button);
    void Initialize(bool background);
private:
    CLayout mControllers;
    CLayout mButtons;
};
}
void InitIconCollection(bool background);
void ShutdownIconCollection();
#endif
