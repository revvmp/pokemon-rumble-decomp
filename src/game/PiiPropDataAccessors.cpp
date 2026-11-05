#define GAME_PIIPROPDATA_ACCESSOR_LAYOUT
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
template <> __declspec(section ".text.piiprop_before_copy") vector<s16>::const_iterator vector<s16>::begin() const {
    return const_iterator(reinterpret_cast<const s16*>(this->data_));
}
template <> __declspec(section ".text.piiprop_before_push") const s16& vector<s16>::const_iterator::operator*() const {
    return *mPointer;
}
template <> __declspec(section ".text.piiprop_before_assign") vector<s16>::const_iterator& vector<s16>::const_iterator::operator++() {
    ++mPointer;
    return *this;
}
template <> __declspec(section ".text.piiprop_before_assign") vector<s16>::const_iterator vector<s16>::end() const {
    return const_iterator(reinterpret_cast<const s16*>(this->data_ + this->size_));
}
template <> __declspec(section ".text.piiprop_before_assign") vector<PiiPropData::SpeciesMoves::LevelMove>::iterator vector<PiiPropData::SpeciesMoves::LevelMove>::begin() {
    return make_iterator(base::begin());
}
template <> __declspec(section ".text.piiprop_before_assign") vector<PiiPropData::SpeciesMoves::LevelMove>::iterator vector<PiiPropData::SpeciesMoves::LevelMove>::end() {
    return make_iterator(base::end());
}
template <> __declspec(section ".text.piiprop_before_assign") vector<s16>::iterator vector<s16>::begin() {
    return make_iterator(base::begin());
}
template <> __declspec(section ".text.piiprop_before_assign") vector<s16>::iterator vector<s16>::end() {
    return make_iterator(base::end());
}
template <> __declspec(section ".text.piiprop_before_assign") PiiPropData::SpeciesMoves& vector<PiiPropData::SpeciesMoves>::operator[](size_type i) {
    return base::operator[](i);
}
template <> __declspec(section ".text.piiprop_late") bool vector<PiiPropData::SpeciesMoves::LevelMove>::const_iterator::operator!=(const const_iterator& rOther) const {
    return base() != rOther.base();
}
template <> __declspec(section ".text.piiprop_late") vector<PiiPropData::SpeciesMoves::LevelMove>::size_type vector<PiiPropData::SpeciesMoves::LevelMove>::size() const {
    return this->size_;
}
template <> __declspec(section ".text.piiprop_reserve") vector<PiiPropData::SpeciesMoves::LevelMove>::const_iterator vector<PiiPropData::SpeciesMoves::LevelMove>::begin() const {
    return const_iterator(reinterpret_cast<const PiiPropData::SpeciesMoves::LevelMove*>(this->data_));
}
template <> __declspec(section ".text.piiprop_tail_end") const PiiPropData::SpeciesMoves::LevelMove* vector<PiiPropData::SpeciesMoves::LevelMove>::const_iterator::operator->() const {
    return mPointer;
}
template <> __declspec(section ".text.piiprop_tail_end") vector<PiiPropData::SpeciesMoves::LevelMove>::const_iterator& vector<PiiPropData::SpeciesMoves::LevelMove>::const_iterator::operator++() {
    ++mPointer;
    return *this;
}
template <> __declspec(section ".text.piiprop_tail_end") vector<PiiPropData::SpeciesMoves::LevelMove>::const_iterator vector<PiiPropData::SpeciesMoves::LevelMove>::end() const {
    return const_iterator(reinterpret_cast<const PiiPropData::SpeciesMoves::LevelMove*>(this->data_ + this->size_));
}
}
