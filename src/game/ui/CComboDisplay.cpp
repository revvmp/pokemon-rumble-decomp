#include <game/ui/LayoutResources.h>
#include <game/text/StringFormat.h>
#include <game/world/IGameMap.h>


#include <game/ui/detail/CComboDisplay.h>

std::tr1::shared_ptr<IComboDisplay> CreateComboDisplay(bool background) {
    std::tr1::shared_ptr<CComboDisplay> pDisplay(new CComboDisplay);
    CComboDisplay* pComboDisplay = pDisplay.get();
    pComboDisplay->Load(GetDefaultFontMap(), background);
    return pDisplay;
}

