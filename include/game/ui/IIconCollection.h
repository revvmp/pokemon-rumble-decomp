#ifndef GAME_IICONCOLLECTION_H
#define GAME_IICONCOLLECTION_H
#include <game/input/IController.h>
#include <nw4r/lyt/lyt_material.h>
class IIconCollection {
public:
    virtual nw4r::lyt::TexMap GetControllerIcon(IController::Type type, int channel) = 0;
    virtual nw4r::lyt::TexMap GetControllerIcon(const IController* pController) = 0;
    virtual nw4r::lyt::TexMap GetButtonIcon(IController::Type type, u32 button) = 0;
    virtual nw4r::lyt::TexMap GetButtonIcon(const IController* pController, u32 button) = 0;
};
IIconCollection* GetIconCollection();
#endif
