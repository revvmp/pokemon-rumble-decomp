#include <game/StageID.h>

const int gStageKindMessageIDs[] = {0x60000, 0x60001, 0x60002, 0};

const StageRankInfo gStageRanks[] = {
    {"D", 0x60003},
    {"C", 0x60004},
    {"B", 0x60005},
    {"A", 0x60006},
    {"S", 0x60007}
};

const StageWorldInfo gStageWorlds[] = {
    {0, "terminal", 0x60015},
    {1, "forest", 0x60016},
    {2, "beach", 0x60017},
    {3, "tower", 0x60018},
    {4, "grassland", 0x60019},
    {5, "cave", 0x6001A},
    {6, "furnace", 0x6001B},
    {7, "battle", 0x6001C}
};
