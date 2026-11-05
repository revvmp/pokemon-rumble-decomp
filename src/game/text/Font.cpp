#include <game/text/Font.h>

namespace {
std::tr1::weak_ptr<CLayout::FontMap> sDefaultFonts;
}

void SetDefaultFontMap(const std::tr1::weak_ptr<CLayout::FontMap>& rFonts) {
    sDefaultFonts = rFonts;
}

std::tr1::shared_ptr<CLayout::FontMap> GetDefaultFontMap() {
    return sDefaultFonts.lock();
}
