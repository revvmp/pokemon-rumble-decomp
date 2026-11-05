#ifndef GAME_PIIPROPDATA_H
#define GAME_PIIPROPDATA_H
#include <game/MapDesc.h>
#include <utility>
#include <vector>
struct PiiPropData {
    struct SpeciesMoves {
#if defined(GAME_PIIPROPDATA_EMISSION_LAYOUT)
        __declspec(section ".text.piiprop_species_begin") SpeciesMoves();
        __declspec(section ".text.piiprop_species_end") ~SpeciesMoves();
#endif
        struct LevelMove {
            s16 mLevel;
            s16 mMove;
        };
        std::vector<LevelMove> mLevelMoves;
        std::vector<s16> mMoveGroups[5];
    };
#if defined(GAME_PIIPROPDATA_EMISSION_LAYOUT)
    __declspec(section ".text.piiprop_head")
#endif
    PiiPropData();
#if defined(GAME_PIIPROPDATA_EMISSION_LAYOUT)
    __declspec(section ".text.piiprop_head")
#endif
    void Reset();
#if defined(GAME_PIIPROPDATA_EMISSION_LAYOUT)
    __declspec(section ".text.piiprop_head")
#endif
    ~PiiPropData();
#if defined(GAME_PIIPROPDATA_EMISSION_LAYOUT)
    __declspec(section ".text.piiprop_head")
#endif
    void Initialize(int columns);
#if defined(GAME_PIIPROPDATA_EMISSION_LAYOUT)
    __declspec(section ".text.piiprop_head")
#endif
    void SetProperty(u16 monsno, u8 form, int formScope, int column, const NPpu::CValue& rValue, const NPpu::CEngine* pEngine);
#if defined(GAME_PIIPROPDATA_EMISSION_LAYOUT)
    __declspec(section ".text.piiprop_head")
#endif
    const NPpu::CValue& GetProperty(u16 monsno, u8 form, int column) const;
#if defined(GAME_PIIPROPDATA_EMISSION_LAYOUT)
    __declspec(section ".text.piiprop_head")
#endif
    void SetWazaLevels(u16 monsno, u8 form, int formScope, const std::vector<s16>& rLevels);
#if defined(GAME_PIIPROPDATA_EMISSION_LAYOUT)
    __declspec(section ".text.piiprop_late")
#endif
    std::vector<s16> GetWazaLevels(u16 monsno, u8 form, int level) const;
    std::vector<SpeciesMoves> mSpecies;
    MapPiiTable mProperties;
};
#endif
