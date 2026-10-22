#include <game/IGameSession.h>
#include <game/PiiPropData.h>
#include <list>
#include <vector>
#include <game/NPpuVM.h>
#include <game/CPpuEngine.h>
#include <game/PiiSpeciesData.h>
#include <game/CPiiCollectionBox.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayer.h>

int RandInt(int minimum, int maximum);
int GetTypeEfficacy(int attackType, int defendType);

struct EvolutionData {
    u16 mTargets[7];
    u16 mCount;
};

extern const EvolutionData gEvolutionData[494];
extern const u16 gRootSpecies[496];

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

int method_pprGetWazaKouka(NPpu::CEngine* pEngine) {
    int type = pEngine->PopInt();
    const PiiSpeciesData* pData = static_cast<CPpuEngine*>(pEngine)->PopSpeciesData();
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
