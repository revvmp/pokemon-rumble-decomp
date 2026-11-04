#include <game/PiiPropData.h>
#include <algorithm>
#include <functional>

namespace std {

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

static bool CompareLevelMoves(const PiiPropData::SpeciesMoves::LevelMove& rA, const PiiPropData::SpeciesMoves::LevelMove& rB) {
    if (rA.mLevel != rB.mLevel) {
        return rA.mLevel < rB.mLevel;
    }
    return rA.mMove < rB.mMove;
}

void PiiPropData::SetWazaLevels(u16 monsno, u8 form, int formScope, const std::vector<s16>& rLevels) {
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
        mSpecies[monsno] = moves;
        for (int i = 1;; ++i) {
            int row = GetPropertyRow(monsno, i);
            if (row == monsno) {
                break;
            }
            mSpecies[row] = moves;
        }
    } else {
        mSpecies[GetPropertyRow(monsno, form)] = moves;
    }
}

const NPpu::CValue& PiiPropData::GetProperty(u16 monsno, u8 form, int column) const {
    return *const_cast<MapPiiTable&>(mProperties).GetCell(GetPropertyRow(monsno, form), column);
}

std::vector<s16> PiiPropData::GetWazaLevels(u16 monsno, u8 form, int level) const {
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
