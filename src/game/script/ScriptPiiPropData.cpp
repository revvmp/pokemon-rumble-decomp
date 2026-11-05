#include <game/session/IGameSession.h>
#include <game/pokemon/PiiPropData.h>
#include <list>
#include <vector>
#include <game/script/NPpuVM.h>
#include <game/script/CPpuEngine.h>
#include <game/pokemon/PiiSpeciesData.h>
#include <game/pokemon/CPiiCollectionBox.h>
#include <game/pokemon/CPiiPersonalData.h>
#include <game/session/CPlayer.h>

int RandInt(int minimum, int maximum);
int GetTypeEfficacy(int attackType, int defendType);

struct EvolutionData {
    u16 mTargets[7];
    u16 mCount;
};

extern const EvolutionData gEvolutionData[494];
extern const u16 gRootSpecies[496];

const EvolutionData gEvolutionData[494] = {
    {{0}, 0},
    {{2}, 1},
    {{3}, 1},
    {{0}, 0},
    {{5}, 1},
    {{6}, 1},
    {{0}, 0},
    {{8}, 1},
    {{9}, 1},
    {{0}, 0},
    {{11}, 1},
    {{12}, 1},
    {{0}, 0},
    {{14}, 1},
    {{15}, 1},
    {{0}, 0},
    {{17}, 1},
    {{18}, 1},
    {{0}, 0},
    {{20}, 1},
    {{0}, 0},
    {{22}, 1},
    {{0}, 0},
    {{24}, 1},
    {{0}, 0},
    {{26}, 0},
    {{0}, 0},
    {{28}, 1},
    {{0}, 0},
    {{30}, 1},
    {{31}, 0},
    {{0}, 0},
    {{33}, 1},
    {{34}, 0},
    {{0}, 0},
    {{36}, 0},
    {{0}, 0},
    {{38}, 0},
    {{0}, 0},
    {{40}, 0},
    {{0}, 0},
    {{42}, 1},
    {{169}, 0},
    {{44}, 1},
    {{45, 182}, 0},
    {{0}, 0},
    {{47}, 1},
    {{0}, 0},
    {{49}, 1},
    {{0}, 0},
    {{51}, 1},
    {{0}, 0},
    {{53}, 1},
    {{0}, 0},
    {{55}, 1},
    {{0}, 0},
    {{57}, 1},
    {{0}, 0},
    {{59}, 0},
    {{0}, 0},
    {{61}, 1},
    {{62, 186}, 0},
    {{0}, 0},
    {{64}, 1},
    {{65}, 0},
    {{0}, 0},
    {{67}, 1},
    {{68}, 0},
    {{0}, 0},
    {{70}, 1},
    {{71}, 0},
    {{0}, 0},
    {{73}, 1},
    {{0}, 0},
    {{75}, 1},
    {{76}, 0},
    {{0}, 0},
    {{78}, 1},
    {{0}, 0},
    {{80, 199}, 1},
    {{0}, 0},
    {{82}, 1},
    {{462}, 0},
    {{0}, 0},
    {{85}, 1},
    {{0}, 0},
    {{87}, 1},
    {{0}, 0},
    {{89}, 1},
    {{0}, 0},
    {{91}, 0},
    {{0}, 0},
    {{93}, 1},
    {{94}, 0},
    {{0}, 0},
    {{208}, 0},
    {{97}, 1},
    {{0}, 0},
    {{99}, 1},
    {{0}, 0},
    {{101}, 1},
    {{0}, 0},
    {{103}, 0},
    {{0}, 0},
    {{105}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{463}, 0},
    {{110}, 1},
    {{0}, 0},
    {{112}, 1},
    {{464}, 0},
    {{242}, 0},
    {{465}, 0},
    {{0}, 0},
    {{117}, 1},
    {{230}, 0},
    {{119}, 1},
    {{0}, 0},
    {{121}, 0},
    {{0}, 0},
    {{0}, 0},
    {{212}, 0},
    {{0}, 0},
    {{466}, 0},
    {{467}, 0},
    {{0}, 0},
    {{0}, 0},
    {{130}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{470, 471, 135, 134, 136, 196, 197}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{233}, 0},
    {{139}, 1},
    {{0}, 0},
    {{141}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{148}, 1},
    {{149}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{153}, 1},
    {{154}, 1},
    {{0}, 0},
    {{156}, 1},
    {{157}, 1},
    {{0}, 0},
    {{159}, 1},
    {{160}, 1},
    {{0}, 0},
    {{162}, 1},
    {{0}, 0},
    {{164}, 1},
    {{0}, 0},
    {{166}, 1},
    {{0}, 0},
    {{168}, 1},
    {{0}, 0},
    {{0}, 0},
    {{171}, 1},
    {{0}, 0},
    {{25}, 0},
    {{35}, 0},
    {{39}, 0},
    {{176}, 0},
    {{468}, 0},
    {{178}, 1},
    {{0}, 0},
    {{180}, 1},
    {{181}, 1},
    {{0}, 0},
    {{0}, 0},
    {{184}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{188}, 1},
    {{189}, 1},
    {{0}, 0},
    {{424}, 0},
    {{192}, 0},
    {{0}, 0},
    {{469}, 0},
    {{195}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{430}, 0},
    {{0}, 0},
    {{429}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{205}, 1},
    {{0}, 0},
    {{0}, 0},
    {{472}, 0},
    {{0}, 0},
    {{210}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{461}, 0},
    {{217}, 1},
    {{0}, 0},
    {{219}, 1},
    {{0}, 0},
    {{221}, 1},
    {{473}, 0},
    {{0}, 0},
    {{224}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{229}, 1},
    {{0}, 0},
    {{0}, 0},
    {{232}, 1},
    {{0}, 0},
    {{474}, 0},
    {{0}, 0},
    {{0}, 0},
    {{107, 106, 237}, 0},
    {{0}, 0},
    {{124}, 1},
    {{125}, 1},
    {{126}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{247}, 1},
    {{248}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{253}, 1},
    {{254}, 1},
    {{0}, 0},
    {{256}, 1},
    {{257}, 1},
    {{0}, 0},
    {{259}, 1},
    {{260}, 1},
    {{0}, 0},
    {{262}, 1},
    {{0}, 0},
    {{264}, 1},
    {{0}, 0},
    {{266, 268}, 0},
    {{267}, 1},
    {{0}, 0},
    {{269}, 1},
    {{0}, 0},
    {{271}, 1},
    {{272}, 0},
    {{0}, 0},
    {{274}, 1},
    {{275}, 0},
    {{0}, 0},
    {{277}, 1},
    {{0}, 0},
    {{279}, 1},
    {{0}, 0},
    {{281}, 1},
    {{282, 475}, 1},
    {{0}, 0},
    {{284}, 1},
    {{0}, 0},
    {{286}, 1},
    {{0}, 0},
    {{288}, 1},
    {{289}, 1},
    {{0}, 0},
    {{291, 292}, 0},
    {{0}, 0},
    {{0}, 0},
    {{294}, 1},
    {{295}, 1},
    {{0}, 0},
    {{297}, 1},
    {{0}, 0},
    {{183}, 0},
    {{476}, 0},
    {{301}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{305}, 1},
    {{306}, 1},
    {{0}, 0},
    {{308}, 1},
    {{0}, 0},
    {{310}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{407}, 0},
    {{317}, 1},
    {{0}, 0},
    {{319}, 1},
    {{0}, 0},
    {{321}, 1},
    {{0}, 0},
    {{323}, 1},
    {{0}, 0},
    {{0}, 0},
    {{326}, 1},
    {{0}, 0},
    {{0}, 0},
    {{329}, 1},
    {{330}, 1},
    {{0}, 0},
    {{332}, 1},
    {{0}, 0},
    {{334}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{340}, 1},
    {{0}, 0},
    {{342}, 1},
    {{0}, 0},
    {{344}, 1},
    {{0}, 0},
    {{346}, 1},
    {{0}, 0},
    {{348}, 1},
    {{0}, 0},
    {{350}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{354}, 1},
    {{0}, 0},
    {{356}, 1},
    {{477}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{202}, 1},
    {{362, 478}, 1},
    {{0}, 0},
    {{364}, 1},
    {{365}, 1},
    {{0}, 0},
    {{367, 368}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{372}, 1},
    {{373}, 1},
    {{0}, 0},
    {{375}, 1},
    {{376}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{388}, 1},
    {{389}, 1},
    {{0}, 0},
    {{391}, 1},
    {{392}, 1},
    {{0}, 0},
    {{394}, 1},
    {{395}, 1},
    {{0}, 0},
    {{397}, 1},
    {{398}, 1},
    {{0}, 0},
    {{400}, 1},
    {{0}, 0},
    {{402}, 1},
    {{0}, 0},
    {{404}, 1},
    {{405}, 1},
    {{0}, 0},
    {{315}, 0},
    {{0}, 0},
    {{409}, 1},
    {{0}, 0},
    {{411}, 1},
    {{0}, 0},
    {{413, 414}, 0},
    {{0}, 0},
    {{0}, 0},
    {{416}, 0},
    {{0}, 0},
    {{0}, 0},
    {{419}, 1},
    {{0}, 0},
    {{421}, 1},
    {{0}, 0},
    {{423}, 1},
    {{0}, 0},
    {{0}, 0},
    {{426}, 1},
    {{0}, 0},
    {{428}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{432}, 1},
    {{0}, 0},
    {{358}, 0},
    {{435}, 1},
    {{0}, 0},
    {{437}, 1},
    {{0}, 0},
    {{185}, 0},
    {{122}, 0},
    {{113}, 0},
    {{0}, 0},
    {{0}, 0},
    {{444}, 1},
    {{445}, 1},
    {{0}, 0},
    {{143}, 0},
    {{448}, 0},
    {{0}, 0},
    {{450}, 1},
    {{0}, 0},
    {{452}, 1},
    {{0}, 0},
    {{454}, 1},
    {{0}, 0},
    {{0}, 0},
    {{457}, 1},
    {{0}, 0},
    {{226}, 0},
    {{460}, 1},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
};

const u16 gRootSpecies[496] = {
    0, 1, 1, 1, 4, 4, 4, 7, 7, 7, 10, 10, 10, 13, 13, 13,
    16, 16, 16, 19, 19, 21, 21, 23, 23, 172, 172, 27, 27, 29, 29, 29,
    32, 32, 32, 173, 173, 37, 37, 174, 174, 41, 41, 43, 43, 43, 46, 46,
    48, 48, 50, 50, 52, 52, 54, 54, 56, 56, 58, 58, 60, 60, 60, 63,
    63, 63, 66, 66, 66, 69, 69, 69, 72, 72, 74, 74, 74, 77, 77, 79,
    79, 81, 81, 83, 84, 84, 86, 86, 88, 88, 90, 90, 92, 92, 92, 95,
    96, 96, 98, 98, 100, 100, 102, 102, 104, 104, 236, 236, 108, 109, 109, 111,
    111, 440, 114, 115, 116, 116, 118, 118, 120, 120, 439, 123, 238, 239, 240, 127,
    128, 129, 129, 131, 132, 133, 133, 133, 133, 137, 138, 138, 140, 140, 142, 446,
    144, 145, 146, 147, 147, 147, 150, 151, 152, 152, 152, 155, 155, 155, 158, 158,
    158, 161, 161, 163, 163, 165, 165, 167, 167, 41, 170, 170, 172, 173, 174, 175,
    175, 177, 177, 179, 179, 179, 43, 298, 298, 438, 60, 187, 187, 187, 190, 191,
    191, 193, 194, 194, 133, 133, 198, 79, 200, 201, 360, 203, 204, 204, 206, 207,
    95, 209, 209, 211, 123, 213, 214, 215, 216, 216, 218, 218, 220, 220, 222, 223,
    223, 225, 458, 227, 228, 228, 116, 231, 231, 137, 234, 235, 236, 236, 238, 239,
    240, 241, 440, 243, 244, 245, 246, 246, 246, 249, 250, 251, 252, 252, 252, 255,
    255, 255, 258, 258, 258, 261, 261, 263, 263, 265, 265, 265, 265, 265, 270, 270,
    270, 273, 273, 273, 276, 276, 278, 278, 280, 280, 280, 283, 283, 285, 285, 287,
    287, 287, 290, 290, 290, 293, 293, 293, 296, 296, 298, 299, 300, 300, 302, 303,
    304, 304, 304, 307, 307, 309, 309, 311, 312, 313, 314, 406, 316, 316, 318, 318,
    320, 320, 322, 322, 324, 325, 325, 327, 328, 328, 328, 331, 331, 333, 333, 335,
    336, 337, 338, 339, 339, 341, 341, 343, 343, 345, 345, 347, 347, 349, 349, 351,
    352, 353, 353, 355, 355, 357, 433, 359, 360, 361, 361, 363, 363, 363, 366, 366,
    366, 369, 370, 371, 371, 371, 374, 374, 374, 377, 378, 379, 380, 381, 382, 383,
    384, 385, 386, 387, 387, 387, 390, 390, 390, 393, 393, 393, 396, 396, 396, 399,
    399, 401, 401, 403, 403, 403, 406, 406, 408, 408, 410, 410, 412, 412, 412, 415,
    415, 417, 418, 418, 420, 420, 422, 422, 190, 425, 425, 427, 427, 200, 198, 431,
    431, 433, 434, 434, 436, 436, 438, 439, 440, 441, 442, 443, 443, 443, 446, 447,
    447, 449, 449, 451, 451, 453, 453, 455, 456, 456, 458, 459, 459, 215, 81, 108,
    111, 114, 239, 240, 175, 193, 133, 133, 207, 220, 137, 280, 299, 355, 361, 479,
    480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 0, 0,
};

int GetPiiProp(NPpu::CEngine* pEngine) {
    int form = pEngine->FetchArgCount() >= 2 ? pEngine->PopInt() : 0;
    const PiiSpeciesData* pData = GetPiiSpeciesData(static_cast<u16>(pEngine->PopInt()), static_cast<u8>(form));
    pEngine->PushInt(reinterpret_cast<int>(pData));
    return 0;
}

int method_pprGetMonsno(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? static_cast<u16>(pData->GetSpecies()) : 0);
    return 0;
}

int method_pprGetFormno(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? static_cast<u8>(pData->GetFormNumber()) : 0);
    return 0;
}

int method_pprGetBoneType(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? pData->GetBoneType() : 0);
    return 0;
}

int method_pprGetFlyHeight(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? pData->GetFlyHeight() : 0);
    return 0;
}

int method_pprGetASize(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? pData->GetASize() : 0);
    return 0;
}

int method_pprGetType1(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? pData->GetPrimaryType() : 0);
    return 0;
}

int method_pprGetType2(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? pData->GetSecondaryType() : 0);
    return 0;
}

int method_pprGetSexVector(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? static_cast<u8>(pData->GetGenderAndTypeFlags()) : 255);
    return 0;
}

int method_pprGetWalkSpeedCoeff(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushFloat(pData != nullptr ? pData->GetWalkSpeedCoeff() : 1.0f);
    return 0;
}

int method_pprGetWalkAnmRate(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushFloat(pData != nullptr ? pData->GetWalkAnmRate() : 1.0f);
    return 0;
}

int method_pprIsMaleOnly(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? static_cast<u8>(pData->GetGenderAndTypeFlags()) == 0 : 0);
    return 0;
}

int method_pprIsFemaleOnly(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? static_cast<u8>(pData->GetGenderAndTypeFlags()) == 254 : 0);
    return 0;
}

int method_pprIsSexUnknown(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? static_cast<u8>(pData->GetGenderAndTypeFlags()) == 255 : 1);
    return 0;
}

int method_pprIsFlier(NPpu::CEngine* pEngine) {
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? pData->GetFlyHeight() != 0 : 0);
    return 0;
}

int method_pprGetWazaKouka(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    NPpu::CValue value;
    pEngine->Pop(value);
    int type = value.ToInt();
    const PiiSpeciesData* pData = pEngine->PopSpeciesData();
    pEngine->PushInt(pData != nullptr ? pData->GetWazaKouka(type) : 0);
    return 0;
}

int InitPiiProps(NPpu::CEngine* pEngine) {
    int columns = pEngine->PopInt();
    IGameSession::GetInstance()->GetPiiPropData()->Initialize(columns);
    pEngine->PushNull();
    return 0;
}

int method_pprSet(NPpu::CEngine* pEngine) {
    NPpu::CValue value;
    pEngine->Pop(value);
    int column = pEngine->PopInt();
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    if (pData != nullptr) {
        PiiPropData* pProps = IGameSession::GetInstance()->GetPiiPropData();
        int form = pData->GetFormNumber();
        int monsno = pData->GetSpecies();
        pProps->SetProperty(static_cast<u16>(monsno), static_cast<u8>(form), 0, column, value, pEngine);
    }
    pEngine->PushNull();
    return 0;
}

int SetPiiProps(NPpu::CEngine* pBaseEngine, int formScope) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    PiiPropData* pProps = IGameSession::GetInstance()->GetPiiPropData();
    int column = pEngine->FetchArgCount() - 1;
    NPpu::CValue identity;
    pEngine->GetArg(column, identity);
    const PiiSpeciesData* pData = reinterpret_cast<const PiiSpeciesData*>(identity.ToInt());
    int monsno = pData->GetSpecies();
    int form = pData->GetFormNumber();
    for (--column; column >= 0; --column) {
        NPpu::CValue value;
        pEngine->Pop(value);
        pProps->SetProperty(static_cast<u16>(monsno), static_cast<u8>(form), formScope, column, value, pEngine);
    }
    pEngine->Pop(identity);
    pEngine->PushNull();
    return 0;
}

int method_pprSetAll(NPpu::CEngine* pEngine) { return SetPiiProps(pEngine, 0); }

int method_pprSetAllForAllFormno(NPpu::CEngine* pEngine) { return SetPiiProps(pEngine, 1); }

int method_pprGet(NPpu::CEngine* pEngine) {
    int column = pEngine->PopInt();
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    if (pData != nullptr) {
        PiiPropData* pProps = IGameSession::GetInstance()->GetPiiPropData();
        int form = pData->GetFormNumber();
        int monsno = pData->GetSpecies();
        NPpu::CValue result = pProps->GetProperty(static_cast<u16>(monsno), static_cast<u8>(form), column);
        pEngine->Push(result);
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int SetPiiWazaLevels(NPpu::CEngine* pEngine, int formScope) {
    int count = pEngine->FetchArgCount();
    std::vector<s16> levels(count - 1);
    while (count >= 2) {
        levels[count - 2] = pEngine->PopInt();
        --count;
    }
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    if (pData != nullptr) {
        PiiPropData* pProps = IGameSession::GetInstance()->GetPiiPropData();
        int form = pData->GetFormNumber();
        int monsno = pData->GetSpecies();
        pProps->SetWazaLevels(static_cast<u16>(monsno), static_cast<u8>(form), formScope, levels);
    }
    pEngine->PushNull();
    return 0;
}

int method_pprSetWazaLevel(NPpu::CEngine* pEngine) { return SetPiiWazaLevels(pEngine, 0); }

int method_pprSetWazaLevelForAllFormno(NPpu::CEngine* pEngine) { return SetPiiWazaLevels(pEngine, 1); }

int method_pprGetWazaLevel(NPpu::CEngine* pEngine) {
    int count = pEngine->FetchArgCount();
    std::list<int> queryLevels;
    for (int i = 3; i < count; ++i) {
        queryLevels.push_front(pEngine->PopInt());
    }
    int size;
    NPpu::CValue* pOutput = pEngine->PopArray(&size);
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
    NPpu::CValue* pBegin = pOutput;
    NPpu::CValue* pEnd = pBegin + size;
    if (size > 0 && pData != nullptr) {
        int monsno = pData->GetSpecies();
        int form = pData->GetFormNumber();
        PiiPropData* pProps = IGameSession::GetInstance()->GetPiiPropData();
        for (std::list<int>::const_iterator query = queryLevels.begin(); query != queryLevels.end(); ++query) {
            std::vector<s16> levels = pProps->GetWazaLevels(static_cast<u16>(monsno), static_cast<u8>(form), *query);
            for (std::vector<s16>::iterator level = levels.begin(); level != levels.end(); ++level) {
                pOutput->SetInt(*level);
                ++pOutput;
                if (pOutput == pEnd) {
                    goto Finished;
                }
            }
        }
    }
Finished:
    if (pOutput != pEnd) {
        pOutput->SetInt(0);
    } else if (pEnd != pBegin) {
        (pEnd - 1)->SetInt(0);
    }
    pEngine->PushInt(pOutput - pBegin);
    return 0;
}

int method_pprGetSexByRnd(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    u32 random = pEngine->FetchArgCount() < 2 ? RandInt(0, 255) : pEngine->PopInt();
    const PiiSpeciesData* pSpecies = pEngine->PopSpeciesData();
    int sex = 2;
    if (pSpecies) {
        int ratio = pSpecies->GetGenderAndTypeFlags() & 0xFF;
        switch (ratio) {
        case 0: sex = 0; break;
        case 254: sex = 1; break;
        case 255: sex = 2; break;
        default: sex = (random & 0xFF) < ratio; break;
        }
    }
    pEngine->PushInt(sex);
    return 0;
}

int method_pprGetCollectionType(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    const PiiSpeciesData* pSpecies = pEngine->PopSpeciesData();
    if (pSpecies) {
        int species = pSpecies->GetSpecies();
        pEngine->PushInt(IGameSession::GetInstance()->GetPiiCollectionBox()->GetCollectionType(species));
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int method_pprSetCollectionBRR(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int value = pEngine->PopInt();
    int type = pEngine->PopInt();
    const PiiSpeciesData* pSpecies = pEngine->PopSpeciesData();
    if (pSpecies) {
        int species = pSpecies->GetSpecies();
        IGameSession::GetInstance()->GetPiiCollectionBox()->SetCollectionBRR(species, type, value);
    }
    pEngine->PushNull();
    return 0;
}

int method_pprGetDefeatCount(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    const PiiSpeciesData* pSpecies = pEngine->PopSpeciesData();
    if (pSpecies) {
        int species = pSpecies->GetSpecies();
        pEngine->PushInt(IGameSession::GetInstance()->GetPiiCollectionBox()->GetDefeatCount(species));
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int GetRootMonsno(NPpu::CEngine* pEngine) {
    int species = pEngine->PopInt();
    pEngine->PushInt(gRootSpecies[species]);
    return 0;
}

int GetNextSinkaMonsno(NPpu::CEngine* pEngine) {
    NPpu::Slice* pOutput = pEngine->PopReference();
    int mode = pEngine->PopInt();
    int species = pEngine->PopInt();
    int count = 0;
    if (mode == 0) {
        for (count = 0; count < gEvolutionData[species].mCount; ++count) {
            pOutput[count].SetInt(gEvolutionData[species].mTargets[count]);
        }
    } else if (mode == 1) {
        for (count = 0; count < 7 && gEvolutionData[species].mTargets[count]; ++count) {
            pOutput[count].SetInt(gEvolutionData[species].mTargets[count]);
        }
    }
    pEngine->PushInt(count);
    return 0;
}

int GetTypeOnTypeEfficacy(NPpu::CEngine* pEngine) {
    int defend = pEngine->PopInt();
    int attack = pEngine->PopInt();
    pEngine->PushInt(GetTypeEfficacy(attack, defend));
    return 0;
}
