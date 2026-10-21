#include <game/LayoutResources.h>
#include <game/StringFormat.h>
#include <game/IGameMap.h>


#include <game/detail/CComboDisplay.h>

std::tr1::shared_ptr<IComboDisplay> CreateComboDisplay(bool background) {
    std::tr1::shared_ptr<CComboDisplay> pDisplay(new CComboDisplay);
    CComboDisplay* pComboDisplay = pDisplay.get();
    pComboDisplay->Load(GetDefaultFontMap(), background);
    return pDisplay;
}

