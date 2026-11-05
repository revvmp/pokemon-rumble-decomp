#define GAME_PIIPROPDATA_ITERATOR_LAYOUT
#pragma section code_type ".text.piiprop_head"
#pragma section code_type ".text.piiprop_tail"
#pragma section code_type ".text.piiprop_species_begin"
#pragma section code_type ".text.piiprop_species_short"
#pragma section code_type ".text.piiprop_species_end"
#pragma section code_type ".text.piiprop_before_copy"
#pragma section code_type ".text.piiprop_short_copy"
#pragma section code_type ".text.piiprop_before_push"
#pragma section code_type ".text.piiprop_push"
#pragma section code_type ".text.piiprop_before_assign"
#pragma section code_type ".text.piiprop_late"
#pragma section code_type ".text.piiprop_reserve"
#pragma section code_type ".text.piiprop_level_copy"
#pragma section code_type ".text.piiprop_tail_end"
#pragma section code_type ".text.piiprop_backends"
#pragma section code_type ".text"
#pragma section code_type ".text"
#include <game/PiiPropData.h>
namespace std {
template <> __declspec(section ".text.piiprop_short_copy")
vector<s16>::const_iterator::const_iterator(const const_iterator& rOther) : mPointer(rOther.mPointer) {}
template <> __declspec(section ".text.piiprop_level_copy")
vector<PiiPropData::SpeciesMoves::LevelMove>::const_iterator::const_iterator(const const_iterator& rOther) : mPointer(rOther.mPointer) {}
}
