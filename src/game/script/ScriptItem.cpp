#include <game/world/CFacility.h>
#include <game/world/BWorld.h>
#include <game/world/CItem.h>
#include <game/script/CPpuEngine.h>
#include <game/world/CSimpleGround.h>
#include <game/world/CWarpZone.h>
#include <game/io/SaveData.h>
#include <game/text/StringFormat.h>
#include <memory>
#include <MSL_C/wide_decimal.h>
#include <revolution/net.h>
#include <string.h>
#include <string>

int method_CSimpleGround_loadModel(NPpu::CEngine* pEngine);

static int AddItem(NPpu::CEngine* pBaseEngine, std::tr1::shared_ptr<CItem> source) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    int argCount = pEngine->FetchArgCount();
    BWorldObj* pParent = nullptr;
    if (argCount >= 2) {
        pParent = pEngine->PopWorldObj();
    }

    const char* pScriptName = nullptr;
    if (argCount >= 1) {
        NPpu::CValue value;
        pEngine->Pop(value);
        if (value.IsString()) {
            pScriptName = pEngine->ToString(value);
        }
    }

    std::tr1::shared_ptr<CItem> item = source;
    item->SwapRegister(pEngine->GetRegister());
    item->Initialize(false);
    if (pParent != nullptr) {
        item->SetParent(pParent->shared_from_this());
    }

    if (pEngine->GetWorld()->Add(item)) {
        if (pScriptName != nullptr) {
            item->LoadScript(pScriptName, 0, std::tr1::shared_ptr<CPpuEnv>());
        }
        pEngine->PushInt(item->GetId());
    } else {
        pEngine->PushInt(0);
    }

    return 0;
}

int NewItem(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CItem> item(new CItem(CItem::TYPE));
    return AddItem(pEngine, item);
}

int NewWarpZone(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CItem> warpZone(new CWarpZone(CWarpZone::TYPE));
    return AddItem(pEngine, warpZone);
}

int NewFacility(NPpu::CEngine* pEngine) {
    std::tr1::shared_ptr<CItem> facility(new CFacility(CFacility::TYPE));
    return AddItem(pEngine, facility);
}

int method_CItem_loadModel(NPpu::CEngine* pEngine) {
    NPpu::CValue value;
    pEngine->Pop(value);
    const char* pName = pEngine->ToString(value);
    CItem* pItem = DynamicCast<CItem>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pItem != nullptr) {
        pItem->LoadModel(pName, false);
    }
    pEngine->PushInt(0);
    return 0;
}

int method_loadModel(NPpu::CEngine* pEngine) {
    if (static_cast<CPpuEngine*>(pEngine)->PeekWorldObjType(1) & CItem::TYPE) {
        return method_CItem_loadModel(pEngine);
    }
    return method_CSimpleGround_loadModel(pEngine);
}

int IsPasswordUsed(NPpu::CEngine* pEngine) {
    NPpu::CValue value;
    pEngine->Pop(value);
    int password = wide_string_to_decimal(pEngine->ToWString(value));
    bool used = false;
    for (std::vector<UsedPassword>::iterator it = GetSaveData(-1)->mUsedPassword.begin(); it != GetSaveData(-1)->mUsedPassword.end(); ++it) {
        if (password == it->mData.mPassword) {
            used = true;
            break;
        }
    }
    pEngine->PushInt(used);
    return 0;
}

int SetPasswordUsed(NPpu::CEngine* pEngine) {
    UsedPassword used;
    NPpu::CValue value;
    pEngine->Pop(value);
    used.mData.mPassword = wide_string_to_decimal(pEngine->ToWString(value));
    GetSaveData(-1)->mUsedPassword.push_back(used);
    pEngine->PushInt(0);
    return 0;
}

int IsSamePassword(NPpu::CEngine* pEngine) {
    u32 expected[5];
    expected[4] = pEngine->PopInt();
    expected[3] = pEngine->PopInt();
    expected[2] = pEngine->PopInt();
    expected[1] = pEngine->PopInt();
    expected[0] = pEngine->PopInt();
    std::string text = FormatString("%08d", pEngine->PopInt());

    char digits[8];
    memcpy(digits, text.c_str(), 8);

    u8 digest[NET_SHA1_DIGEST_SIZE];
    NETSHA1Context context;
    NETSHA1Init(&context);
    NETSHA1Update(&context, digits, 8);
    NETSHA1GetDigest(&context, digest);

    NETSHA1Context rehash;
    NETSHA1Init(&rehash);
    NETSHA1Update(&rehash, digest, NET_SHA1_DIGEST_SIZE);
    NETSHA1GetDigest(&rehash, digest);

    pEngine->PushInt(memcmp(digest, expected, NET_SHA1_DIGEST_SIZE) == 0);
    return 0;
}
