#define GAME_PIIPROPDATA_EMISSION_LAYOUT
#define GAME_PIIPROPDATA_COPY_LAYOUT
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
#pragma section code_type ".text.piiprop_tail"
#include <MSL_CPP/msl_utility.h>
struct PiiPropertyAllocationTag : Metrowerks::int2type<false> { PiiPropertyAllocationTag() {}  };
#pragma section code_type ".text.piiprop_head"
#include <algorithm>
#pragma section code_type ".text.piiprop_tail"
#include <game/PiiPropData.h>
#include <algorithm>
#include <functional>

#pragma section code_type ".text.piiprop_head"
namespace std {
template <> __declspec(section ".text.piiprop_species_short") inline vector<s16>::vector() {}
template <> __declspec(section ".text.piiprop_species_begin") inline bool vector<s16>::const_iterator::operator!=(const const_iterator& rOther) const {
    return base() != rOther.base();
}

template <> __declspec(section ".text.piiprop_head") inline vector<PiiPropData::SpeciesMoves>::~vector() {}

template <class T, bool IsPOD = Metrowerks::is_POD<T>::value> struct __msl_copy {
    static T* copy(T* first, T* last, T* result) {
        for (; first < last; ++first, ++result) {
            *result = *first;
        }
        return result;
    }
};

template <class T> struct __msl_copy<T, true> {
    static T* copy(T* first, T* last, T* result) {
        size_t n = static_cast<size_t>(last - first);
        memmove(result, first, n * sizeof(T));
        return result + n;
    }
};

template <class T> inline T* copy(T* first, T* last, T* result) {
    return __msl_copy<T>::copy(first, last, result);
}

template <class T> inline T* copy(const T* first, const T* last, T* result) {
    return __msl_copy<T>::copy(const_cast<T*>(first), const_cast<T*>(last), result);
}

template <>
inline void __vec_deleter<unsigned short, allocator<unsigned short> >::construct_at_end(
    const unsigned short* first, const unsigned short* last, Metrowerks::int2type<true>) {
    std::copy(first, last, data_ + size_);
    size_ += size_type(last - first);
}

template <> template <>
void __vec_deleter<PiiPropData::SpeciesMoves::LevelMove, allocator<PiiPropData::SpeciesMoves::LevelMove> >::init<
    const PiiPropData::SpeciesMoves::LevelMove*>(
    const PiiPropData::SpeciesMoves::LevelMove* first,
    const PiiPropData::SpeciesMoves::LevelMove* last, forward_iterator_tag) {
    size_type n = (size_type)std::distance(first, last);
    if (n > 0) {
        allocate(n);
        construct_at_end(first, last);
    }
}

template <>
inline __vec_constructor<PiiPropData::SpeciesMoves::LevelMove, allocator<PiiPropData::SpeciesMoves::LevelMove>, false>::__vec_constructor(
    const __vec_constructor& other) : base(other.alloc()) {
    base::init(other.begin(), other.end(), random_access_iterator_tag());
}

template <> template <>
void __vec_deleter<unsigned short, allocator<unsigned short> >::init<const unsigned short*>(
    const unsigned short* first, const unsigned short* last, forward_iterator_tag) {
    size_type n = (size_type)std::distance(first, last);
    if (n > 0) {
        allocate(n);
        construct_at_end(first, last);
    }
}

template <>
inline __vec_constructor<unsigned short, allocator<unsigned short>, false>::__vec_constructor(
    const __vec_constructor& other) : base(other.alloc()) {
    base::init(other.begin(), other.end(), random_access_iterator_tag());
}

template <> template <>
inline __vec_constructor<PiiPropData::SpeciesMoves, allocator<PiiPropData::SpeciesMoves>, false>::__vec_constructor(
    size_type n, const ::PiiPropertyAllocationTag& allocationTag) : base() {
    if (n != 0) {
        *reinterpret_cast<volatile unsigned char*>(&const_cast<PiiPropertyAllocationTag&>(allocationTag)) = 0;
        allocate(n, static_cast<const pod_type&>(allocationTag));
        base::construct_at_end(n, pod_type());
    }
}

template <>
inline vector<PiiPropData::SpeciesMoves, allocator<PiiPropData::SpeciesMoves> >::vector(size_type n)
    : base(n, PiiPropertyAllocationTag()) {}

inline void ThrowShortVectorLengthError() {
    __vector_common<true>::throw_length_error();
}

inline void InitializeShortVectorTag(Metrowerks::int2type<1>) {}

template <> inline __vec_deleter<u16, allocator<u16> >::size_type
__vec_deleter<u16, allocator<u16> >::recommend(size_type n) {
    InitializeShortVectorTag(pod_type());
    size_type firstGrowth;
    size_type secondGrowth;
    size_type requestedGrowth = n;
    size_type maxSize = this->max_size();
    size_type capacity = this->cap();
    if (n > maxSize - capacity) {
        this->throw_length_error();
    }
    size_type third = maxSize / 3;
    if (capacity < third) {
        firstGrowth = (capacity + 1) * 3 / 5;
        return capacity + max(firstGrowth, requestedGrowth);
    }
    if (capacity < third * 2) {
        secondGrowth = (capacity + 1) / 2;
        return capacity + max(secondGrowth, requestedGrowth);
    }
    return maxSize;
}

inline void AllocateShortVectorStorage(__vec_deleter<u16, allocator<u16> >* pStorage,
                                      unsigned long n, Metrowerks::int2type<true>) {
    if (n > pStorage->max_size()) {
        ThrowShortVectorLengthError();
    }
    pStorage->data_ = pStorage->alloc().allocate(n, 0);
    pStorage->cap() = n;
}

inline void AllocateShortVectorStorage(__vec_deleter<u16, allocator<u16> >* pStorage,
                                      unsigned long n) {
    AllocateShortVectorStorage(pStorage, n, Metrowerks::int2type<true>());
}

inline void PrepareShortVectorStorage(__vec_deleter<u16, allocator<u16> >* pStorage,
                                     unsigned long capacity) {
    pStorage->clear();
    if (pStorage->data_) {
        pStorage->alloc().deallocate(pStorage->data_, pStorage->cap());
        pStorage->data_ = nullptr;
        pStorage->cap() = 0;
    }
    AllocateShortVectorStorage(pStorage, capacity);
}

template <> template <> inline void
__vec_constructor<u16, allocator<u16>, false>::assign<const u16*>(
    const u16* first, const u16* last, Metrowerks::int2type<true>) {
    size_type n = std::distance(first, last);
    if (n > this->cap()) {
        size_type capacity = recommend(n - this->cap());
        PrepareShortVectorStorage(this, capacity);
    }
    copy_assign(first, last, this->data_);
    this->size_ = n;
}

}

static int GetPropertyRow(int monsno, int form) {
    switch (monsno) {
    case 386:
        if (form != 0 && form <= 3) {
            return form + 495;
        }
        break;
    case 413:
        if (form != 0 && form <= 2) {
            return form + 498;
        }
        break;
    case 487:
        if (form != 0 && form <= 1) {
            return form + 500;
        }
        break;
    case 492:
        if (form != 0 && form <= 1) {
            return form + 501;
        }
        break;
    case 479:
        if (form != 0 && form <= 5) {
            return form + 502;
        }
        break;
    }
    return monsno;
}

PiiPropData::PiiPropData() : mSpecies(508) {}

PiiPropData::~PiiPropData() {}

void PiiPropData::Reset() {
    std::vector<SpeciesMoves> species(508);
    mSpecies.swap(species);
    mProperties.Clear();
}

#pragma dont_inline on

void PiiPropData::Initialize(int columns) {
    mProperties.Initialize(508, columns);
}

void PiiPropData::SetProperty(u16 monsno, u8 form, int formScope, int column, const NPpu::CValue& rValue, const NPpu::CEngine* pEngine) {
    if (formScope == 1) {
        mProperties.SetValue(monsno, column, rValue, pEngine);
        for (int i = 1;; ++i) {
            int row = GetPropertyRow(monsno, i);
            if (row == monsno) {
                break;
            }
            mProperties.SetValue(row, column, rValue, pEngine);
        }
    } else {
        mProperties.SetValue(GetPropertyRow(monsno, form), column, rValue, pEngine);
    }
}

const NPpu::CValue& PiiPropData::GetProperty(u16 monsno, u8 form, int column) const {
    return *const_cast<MapPiiTable&>(mProperties).GetCell(GetPropertyRow(monsno, form), column);
}

static bool CompareLevelMoves(const PiiPropData::SpeciesMoves::LevelMove& rA, const PiiPropData::SpeciesMoves::LevelMove& rB) {
    if (rA.mLevel != rB.mLevel) {
        return rA.mLevel < rB.mLevel;
    }
    return rA.mMove < rB.mMove;
}

template <typename T> inline T& AssignPropertyMoves(T& rDestination, const T& rSource);

void PiiPropData::SetWazaLevels(u16 monsno, u8 form, int formScope, const std::vector<s16>& rLevels) {
    typedef std::vector<s16>::const_iterator PropertyIterator;
    if (false) {
        PropertyIterator* pIterator = nullptr;
        pIterator->operator!=(*pIterator);
    }

    SpeciesMoves moves;
    int group = 0;
    SpeciesMoves::LevelMove pending = {0, 0};
    for (std::vector<s16>::const_iterator it = rLevels.begin(); it != rLevels.end(); ++it) {
        s16 value = *it;
        if (value < 0) {
            group = value;
        } else if (group == -6) {
            if (pending.mLevel == 0) {
                pending.mLevel = value;
            } else {
                pending.mMove = value;
                moves.mLevelMoves.push_back(pending);
                pending.mLevel = 0;
            }
        } else {
            moves.mMoveGroups[group + 5].push_back(value);
        }
    }
    std::sort(moves.mLevelMoves.begin(), moves.mLevelMoves.end(), CompareLevelMoves);
    for (int i = 0; i < 5; ++i) {
        std::sort(moves.mMoveGroups[i].begin(), moves.mMoveGroups[i].end());
    }
    if (formScope == 1) {
        AssignPropertyMoves(mSpecies[monsno], moves);
        for (int i = 1;; ++i) {
            int row = GetPropertyRow(monsno, i);
            if (row == monsno) {
                break;
            }
            AssignPropertyMoves(mSpecies[row], moves);
        }
    } else {
        AssignPropertyMoves(mSpecies[GetPropertyRow(monsno, form)], moves);
    }
}
#pragma dont_inline off
#pragma section code_type ".text.piiprop_tail"
inline PiiPropData::SpeciesMoves::SpeciesMoves() {}
namespace std {
template <> __declspec(section ".text.piiprop_species_begin") inline vector<PiiPropData::SpeciesMoves::LevelMove>::~vector() {}
template <> __declspec(section ".text.piiprop_species_end") inline vector<s16>::~vector() {}
}
inline PiiPropData::SpeciesMoves::~SpeciesMoves() {}
namespace std {
template <> __declspec(section ".text.piiprop_push") void vector<PiiPropData::SpeciesMoves::LevelMove>::push_back(const PiiPropData::SpeciesMoves::LevelMove& x) {
    base::push_back(x);
}
template <> __declspec(section ".text.piiprop_push") void vector<s16>::push_back(const s16& x) {
    base::push_back(x);
}
}
#pragma dont_inline reset

#pragma dont_inline off
template <typename T> inline T& AssignPropertyMoveMembers(T& rDestination, const T& rSource) {
    return rDestination = rSource;
}
template <typename T> inline T& AssignPropertyMoves(T& rDestination, const T& rSource) {
    return AssignPropertyMoveMembers(rDestination, rSource);
}
#pragma dont_inline reset

#pragma dont_inline off
#pragma section code_type ".text.piiprop_late"
template PiiPropData::SpeciesMoves& AssignPropertyMoves<PiiPropData::SpeciesMoves>(PiiPropData::SpeciesMoves&, const PiiPropData::SpeciesMoves&);
#pragma section code_type ".text.piiprop_tail"
#pragma dont_inline reset
std::vector<s16> PiiPropData::GetWazaLevels(u16 monsno, u8 form, int level) const {
    typedef std::vector<SpeciesMoves::LevelMove>::const_iterator PropertyIterator;
    if (false) {
        PropertyIterator* pIterator = nullptr;
        pIterator->operator!=(*pIterator);
    }

    const SpeciesMoves& rMoves = const_cast<std::vector<SpeciesMoves>&>(mSpecies)[GetPropertyRow(monsno, form)];
    std::vector<s16> result;
    if (1 <= level && level <= 100) {
        result.reserve(rMoves.mLevelMoves.size());
        for (std::vector<SpeciesMoves::LevelMove>::const_iterator it = rMoves.mLevelMoves.begin(); it != rMoves.mLevelMoves.end(); ++it) {
            if (level < it->mLevel) {
                break;
            }
            result.push_back(it->mMove);
        }
    } else if (level == -6) {
        result.reserve(rMoves.mLevelMoves.size());
        for (std::vector<SpeciesMoves::LevelMove>::const_iterator it = rMoves.mLevelMoves.begin(); it != rMoves.mLevelMoves.end(); ++it) {
            result.push_back(it->mMove);
        }
    } else if (-5 <= level && level < 0) {
        result = rMoves.mMoveGroups[level + 5];
    }
    return result;
}

#pragma dont_inline reset
