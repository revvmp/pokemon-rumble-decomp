#include <game/PiiPropData.h>
#include <algorithm>
#include <functional>

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
    return *mProperties.GetCell(GetPropertyRow(monsno, form), column);
}

std::vector<s16> PiiPropData::GetWazaLevels(u16 monsno, u8 form, int level) const {
    const SpeciesMoves& rMoves = mSpecies[GetPropertyRow(monsno, form)];
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
