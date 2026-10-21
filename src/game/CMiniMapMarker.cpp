#include <game/CMiniMapMarker.h>
#include <game/LayoutResources.h>

CMiniMapMarker::CMiniMapMarker() : mType(0), mpMarker(nullptr) {}

CMiniMapMarker::~CMiniMapMarker() {}

void CMiniMapMarker::Initialize(int type, const char* pLayoutName, bool background) {
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    mType = type;
    mLayout.Load(gSharedLayoutArchives, pLayoutName, fonts, background);
    mpMarker = mLayout.FindPane("root");
}
