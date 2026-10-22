#include <game/BUILayout.h>

BUILayout::BUILayout() {}

BUILayout::~BUILayout() {}

void BUILayout::Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background) {
    mLayout.Load(pArchive, pLayoutName, rFonts, background);
}

void BUILayout::Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background) {
    mLayout.Load(pArchives, pLayoutName, rFonts, background);
}

bool BUILayout::AttachChild(const char* pPaneName, std::tr1::shared_ptr<BUILayout> child) {
    if (!AddChild(child)) {
        return false;
    }
    return mLayout.AttachLayout(pPaneName, &child->mLayout);
}

void BUILayout::Calc() {
    mLayout.Calc();
    BUI::Calc();
}

void BUILayout::Draw() {
    if (mFlags & 2) {
        mLayout.Draw();
        BUI::Draw();
    }
}
