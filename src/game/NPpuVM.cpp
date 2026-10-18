#include <game/NPpuVM.h>

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

namespace NPpu {

CCode::CCode() : mCode(nullptr), mStrings(nullptr), mWStrings(nullptr) {}

inline void CCode::Clear() {
    if (mStrings != nullptr) {
        free(mStrings);
    }
    mStrings = nullptr;
    if (mWStrings != nullptr) {
        free(mWStrings);
    }
    mWStrings = nullptr;
    mCode = nullptr;
}

CCode::~CCode() {
    Clear();
}

bool CCode::Load(const char* pName, const void* pData, u32, int, int) {
    const SHeader* pHeader = static_cast<const SHeader*>(pData);

    Clear();
    mName = pName;

    switch (pHeader->mMagic) {
    case MAGIC_OLD:
    case MAGIC:
        break;
    case MAGIC_SWAPPED:
        mName = nullptr;
        return false;
    case MAGIC_OLD_SWAPPED:
        mName = nullptr;
        return false;
    default:
        mName = nullptr;
        return false;
    }

    mStackSize = pHeader->mStackSize;
    mCodeSize = pHeader->mCodeSize;
    mStringCount = pHeader->mStringCount;
    mWStringCount = pHeader->mWStringCount;
    mCode = reinterpret_cast<const u32*>(pHeader + 1);

    u32 size = mWStringCount * sizeof(const wchar_t*);
    if (size != 0) {
        mWStrings = static_cast<const wchar_t**>(malloc(size));
        if (pHeader->mWStringDataSize != 0) {
            const wchar_t* p = reinterpret_cast<const wchar_t*>(static_cast<const u8*>(pData) + mCodeSize + sizeof(SHeader));
            for (int i = 0; i < mWStringCount; i++) {
                mWStrings[i] = p;
                while (*p++ != L'\0') {}
            }
        }
    }

    size = mStringCount * sizeof(const char*);
    if (size != 0) {
        mStrings = static_cast<const char**>(malloc(size));
        if (pHeader->mStringDataSize != 0) {
            const char* p = static_cast<const char*>(pData) + (mCodeSize + pHeader->mWStringDataSize) + sizeof(SHeader);
            for (int i = 0; i < mStringCount; i++) {
                mStrings[i] = p;
                while (*p++ != '\0') {}
            }
        }
    }

    return true;
}

int CValue::ToInt() const {
    if (IsInt()) {
        return mInt;
    }
    if (mType == TYPE_FLOAT) {
        return mFloat;
    }
    return 0;
}

bool CValue::IsFloat() const {
    return mType == TYPE_FLOAT;
}

bool CValue::IsNull() const {
    return mType == TYPE_REFERENCE && mReference == nullptr;
}

bool CValue::ToBool() const {
    if (IsInt()) {
        return mInt != 0;
    }
    if (mType == TYPE_FLOAT) {
        return mFloat;
    }
    if (mType == TYPE_REFERENCE) {
        return !IsNull();
    }
    return true;
}

f32 CValue::ToFloat() const {
    if (mType == TYPE_FLOAT) {
        return mFloat;
    }
    if (IsInt()) {
        return mInt;
    }
    return 0.0f;
}

CValue* CValue::GetReference() const {
    return mReference;
}

int CVM::ParseInt(const CValue& rValue) const {
    if (!rValue.IsString()) {
        return 0;
    }
    return atoi(rValue.mType == CValue::TYPE_STRING_PTR ? rValue.mString : mCode->GetString(rValue.mInt));
}

int CVM::ParseWInt(const CValue& rValue) const {
    if (!rValue.IsWString()) {
        return 0;
    }
    return wcstol(rValue.mType == CValue::TYPE_WSTRING_PTR ? rValue.mWString : mCode->GetWString(rValue.mInt), nullptr, 10);
}

void CValue::Increment() {
    if (IsInt()) {
        int value = ToInt();
        mType = TYPE_INT;
        mInt = value + 1;
    } else {
        switch (mType) {
        case TYPE_FLOAT:
            {
                f32 value = mFloat;
                mType = TYPE_FLOAT;
                mFloat = 1.0f + value;
            }
            break;
        case TYPE_REFERENCE:
            if (!IsNull()) {
                CValue* pValue = mReference;
                mType = TYPE_REFERENCE;
                mReference = pValue + 1;
            }
            break;
        }
    }
}

void CValue::SetReference(CValue* pValue) {
    mType = TYPE_REFERENCE;
    mReference = pValue;
}

void CValue::Decrement() {
    if (IsInt()) {
        int value = ToInt();
        mType = TYPE_INT;
        mInt = value - 1;
    } else {
        switch (mType) {
        case TYPE_FLOAT:
            {
                f32 value = mFloat;
                mType = TYPE_FLOAT;
                mFloat = value - 1.0f;
            }
            break;
        case TYPE_REFERENCE:
            if (!IsNull()) {
                CValue* pValue = mReference;
                mType = TYPE_REFERENCE;
                mReference = pValue - 1;
            }
            break;
        }
    }
}

inline void CEnvironment::AllocGlobals() {
    if (mGlobals != nullptr) {
        free(mGlobals);
    }
    if (mGlobalCount != 0) {
        u32 size = mGlobalCount * sizeof(CValue);
        mGlobals = static_cast<CValue*>(malloc(size));
        memset(mGlobals, 0, size);
    }
}

CEnvironment::CEnvironment(int globalCount, const SNativeFunction* pNatives, u32 nativeCount) :
    mGlobals(nullptr), mNativeCount(0), mNatives(nullptr) {
    mGlobalCount = globalCount;
    AllocGlobals();
    mNativeCount = nativeCount;
    mNatives = pNatives;
}

CEnvironment::~CEnvironment() {
    if (mGlobals != nullptr) {
        free(mGlobals);
    }
}

const SNativeFunction* CEnvironment::GetNative(int index) const {
    if (index >= 0 && index <= mNativeCount) {
        return &mNatives[index];
    }
    return nullptr;
}

int CVM::sNextID = 1;
std::set<char*>* CVM::sStrings;
std::set<wchar_t*>* CVM::sWStrings;
std::set<CVM*>* CVM::sInstances;

CVM::CVM() : mID(sNextID++), mCode(nullptr), mEnv(nullptr), mState(0) {
    mBuffer = nullptr;
    mStack = nullptr;
    mFrames = nullptr;
    mReturns = nullptr;
    mBases = nullptr;
    mHandlers = nullptr;
    mThis.mType = CValue::TYPE_INT;
    mThis.mInt = 0;
    mThrown.mType = CValue::TYPE_INT;
    mThrown.mInt = 0;
}

CVM::~CVM() {
    sInstances->erase(sInstances->find(this));
    if (mBuffer != nullptr) {
        free(mBuffer);
    }
    if (mEnv != nullptr) {
        mEnv->Release();
    }
    if (mCode != nullptr) {
        mCode->Release();
    }
}

void CVM::Init(CCode* pCode, CEnvironment* pEnv, int frameCapacity, int baseCount, int handlerCapacity) {
    if (sStrings == nullptr) {
        sStrings = new std::set<char*>;
    }
    if (sWStrings == nullptr) {
        sWStrings = new std::set<wchar_t*>;
    }
    if (sInstances == nullptr) {
        sInstances = new std::set<CVM*>;
    }
    std::pair<std::set<CVM*>::iterator, bool> result = sInstances->insert(this);

    pCode->AddRef();
    mCode = pCode;
    pEnv->AddRef();
    mEnv = pEnv;
    mFrameCapacity = frameCapacity;
    mBaseCount = baseCount;
    mHandlerCapacity = handlerCapacity;

    mBuffer = malloc(mCode->GetStackSize() * sizeof(CValue) + frameCapacity * sizeof(int) + frameCapacity * sizeof(SFrame) +
                     handlerCapacity * sizeof(SHandler) + baseCount * sizeof(int));
    mHandlers = static_cast<SHandler*>(mBuffer);
    mBases = reinterpret_cast<int*>(mHandlers + mHandlerCapacity);
    mReturns = mBases + mBaseCount;
    mFrames = reinterpret_cast<SFrame*>(mReturns + mFrameCapacity);
    mStack = reinterpret_cast<CValue*>(mFrames + mFrameCapacity);

    mState = STATE_IDLE;
    mPC = 0;
    mSP = 0;
    memset(mStack, 0, mCode->GetStackSize() * sizeof(CValue));
    mFrameCount = 0;
    mReturnCount = 0;
    memset(mReturns, 0, mFrameCapacity * sizeof(int));
    memset(mBases, 0, mBaseCount * sizeof(int));
    mHandlerCount = 0;
    mInterrupted = false;
}

}
