#ifndef GAME_FONT_H
#define GAME_FONT_H

#include <game/ui/CLayout.h>

void SetDefaultFontMap(const std::tr1::weak_ptr<CLayout::FontMap>& rFonts);
std::tr1::shared_ptr<CLayout::FontMap> GetDefaultFontMap();

#endif
