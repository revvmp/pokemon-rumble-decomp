#ifndef GAME_PIIPROPDATA_H
#define GAME_PIIPROPDATA_H
#include <game/MapDesc.h>
#include <utility>
#include <vector>
struct PiiPropData {
    struct SpeciesMoves {
        struct LevelMove {
            s16 mLevel;
            s16 mMove;
        };
        std::vector<LevelMove> mLevelMoves;
        std::vector<s16> mMoveGroups[5];
    };
    PiiPropData();
    void Reset();
    ~PiiPropData();
    void Initialize(int columns);
    void SetProperty(u16 monsno, u8 form, int formScope, int column, const NPpu::CValue& rValue, const NPpu::CEngine* pEngine);
    const NPpu::CValue& GetProperty(u16 monsno, u8 form, int column) const;
    void SetWazaLevels(u16 monsno, u8 form, int formScope, const std::vector<s16>& rLevels);
    std::vector<s16> GetWazaLevels(u16 monsno, u8 form, int level) const;
    mutable std::vector<SpeciesMoves> mSpecies;
    MapPiiTable mProperties;
};
#endif
