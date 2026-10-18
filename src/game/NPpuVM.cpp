#include <game/NPpuVM.h>

#include <revolution/enc.h>
#include <stdio.h>
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

int CEngine::ParseInt(const CValue& rValue) const {
    if (!rValue.IsString()) {
        return 0;
    }
    const char* pString;
    if (rValue.mType == CValue::TYPE_STRING_PTR) {
        pString = rValue.mString;
    } else {
        int index = rValue.mInt;
        if (mCode->mStrings != nullptr && index >= 0 && index < mCode->mStringCount) {
            pString = mCode->mStrings[index];
        } else {
            pString = nullptr;
        }
    }
    return atoi(pString);
}

int CEngine::ParseWInt(const CValue& rValue) const {
    if (!rValue.IsWString()) {
        return 0;
    }
    const wchar_t* pString;
    if (rValue.mType == CValue::TYPE_WSTRING_PTR) {
        pString = rValue.mWString;
    } else {
        int index = rValue.mInt;
        if (mCode->mWStrings != nullptr && index >= 0 && index < mCode->mWStringCount) {
            pString = mCode->mWStrings[index];
        } else {
            pString = nullptr;
        }
    }
    return wcstol(pString, nullptr, 10);
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

int CEngine::sNextID = 1;
int CEngine::sGCMark;
int CEngine::sGCAllocCount;
std::set<SGCStringRef<char> >* CEngine::sStrings;
std::set<SGCStringRef<wchar_t> >* CEngine::sWStrings;
std::set<CEngine*>* CEngine::sInstances;

CEngine::CEngine() : mID(sNextID++), mCode(nullptr), mEnv(nullptr), mState(0) {
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

CEngine::~CEngine() {
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

void CEngine::Init(CCode* pCode, CEnvironment* pEnv, int frameCapacity, int baseCount, int handlerCapacity) {
    if (sStrings == nullptr) {
        sStrings = new std::set<SGCStringRef<char> >;
    }
    if (sWStrings == nullptr) {
        sWStrings = new std::set<SGCStringRef<wchar_t> >;
    }
    if (sInstances == nullptr) {
        sInstances = new std::set<CEngine*>;
    }
    std::pair<std::set<CEngine*>::iterator, bool> result = sInstances->insert(this);

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

enum Opcode {
    OP_NOP = 0x00,
    OP_LINE = 0x01,
    OP_NONE = 0x02,
    OP_NEG = 0x03,
    OP_BITNOT = 0x04,
    OP_ADD = 0x05,
    OP_SUB = 0x06,
    OP_MUL = 0x07,
    OP_DIV = 0x08,
    OP_MOD = 0x09,
    OP_STORE = 0x0A,
    OP_EQ = 0x0B,
    OP_NE = 0x0C,
    OP_GT = 0x0D,
    OP_LT = 0x0E,
    OP_GE = 0x0F,
    OP_LE = 0x10,
    OP_NOT = 0x11,
    OP_LAND = 0x12,
    OP_LOR = 0x13,
    OP_AND = 0x14,
    OP_XOR = 0x15,
    OP_OR = 0x16,
    OP_SHR = 0x17,
    OP_SHL = 0x18,
    OP_LOAD = 0x19,
    OP_CALL = 0x1A,
    OP_CALL_NATIVE = 0x1B,
    OP_JUMP = 0x1C,
    OP_JUMP_FALSE = 0x1D,
    OP_RETURN = 0x1E,
    OP_POP = 0x1F,
    OP_ENTER = 0x20,
    OP_LOAD_ELEMENT = 0x21,
    OP_STORE_ELEMENT = 0x22,
    OP_END = 0x23,
    OP_JUMP_TRUE = 0x24,
    OP_JUMP_FALSE_KEEP = 0x25,
    OP_JUMP_TRUE_KEEP = 0x26,
    OP_PUSH_THIS = 0x27,
    OP_CALL_NATIVE_2 = 0x28,
    OP_IN_RANGE = 0x29,
    OP_SELECT = 0x2A,
    OP_DUP = 0x2B,
    OP_CALL_INDIRECT = 0x2C,
    OP_PRE_INC = 0x2D,
    OP_POST_INC = 0x2E,
    OP_PRE_DEC = 0x2F,
    OP_POST_DEC = 0x30,
    OP_DEREF = 0x31,
    OP_ADDRESS = 0x32,
    OP_ADDRESS_ELEMENT = 0x33,
    OP_STORE_DEREF = 0x34,
    OP_PUSH_NULL = 0x35,
    OP_CHECK_ARG = 0x36,
    OP_INDEX_2 = 0x37,
    OP_INDEX_3 = 0x38,
    OP_INDEX_4 = 0x39,
    OP_LOAD_INDIRECT = 0x3A,
    OP_ADDRESS_INDIRECT = 0x3B,
    OP_STORE_INDIRECT = 0x3C,
    OP_TYPEOF = 0x3D,
    OP_SWAP = 0x3E,
    OP_PUSH_INT = 0x3F,
    OP_PUSH_SHORT = 0x40,
    OP_PUSH_FLOAT = 0x41,
    OP_PUSH_STRING = 0x42,
    OP_PUSH_WSTRING = 0x43,
    OP_PUSH_FUNCTION = 0x44,
    OP_TRY = 0x45,
    OP_END_TRY = 0x46,
    OP_PUSH_THROWN = 0x47,
    OP_POP_THROWN = 0x48,
    OP_THROW = 0x49,
    OP_NOP_2 = 0x4A,
    OP_SWITCH = 0x4B,
    OP_TABLE_SWITCH = 0x4C,
    OP_CASE = 0x4D
};

void CEngine::SetThis(const CValue& rValue) {
    mThis = rValue;
}

void CEngine::GetThis(CValue& rValue) const {
    rValue = mThis;
}

CValue* CEngine::PopArray(int* pCount) {
    *pCount = CValue(mStack[--mSP]).ToInt();
    return mStack[--mSP].GetReference();
}

bool CEngine::IsIntArg() const {
    return mStack[mSP - 1].IsInt();
}

bool CEngine::IsStringArg() const {
    return mStack[mSP - 1].IsString();
}

void CEngine::GetArg(int index, CValue& rValue) const {
    rValue = mStack[mSP - index - 1];
}

int CEngine::GetArgInt(int index) const {
    return CValue(mStack[mSP - index - 1]).ToInt();
}

void CEngine::PushFrame(int baseIndex) {
    mFrames[mFrameCount].mSP = mSP;
    mFrames[mFrameCount].mBaseIndex = baseIndex;
    mFrames[mFrameCount++].mSavedBase = mBases[baseIndex];
}

void CEngine::PopFrame() {
    while (mHandlerCount > 0) {
        SHandler& rHandler = mHandlers[mHandlerCount - 1];
        if (rHandler.mFrameCount != mFrameCount) {
            break;
        }
        mHandlerCount--;
    }
    SFrame& rFrame = mFrames[--mFrameCount];
    mSP = rFrame.mSP;
    mBases[rFrame.mBaseIndex] = rFrame.mSavedBase;
    if (mReturnCount > 0) {
        mPC = mReturns[--mReturnCount];
    }
}

int CEngine::Run() {
    switch (mState) {
    case STATE_IDLE:
    case STATE_WAIT:
        mState = Execute(false);
        break;
    case STATE_READY:
    case STATE_END:
        break;
    default:
        mState = STATE_END;
        break;
    }
    return mState;
}

bool CEngine::Reset() {
    if (mState < STATE_IDLE || mState > STATE_READY) {
        return false;
    }
    mPC = 0;
    mSP = 0;
    mFrameCount = 0;
    mReturnCount = 0;
    mState = STATE_READY;
    return true;
}

bool CEngine::Call(CValue* pResult, int pc, int argCount, const CValue* pArgs) {
    switch (mState) {
    case STATE_IDLE:
    case STATE_WAIT:
        break;
    default:
        return false;
    }
    for (int i = 0; i < argCount; i++) {
        Push(pArgs[i]);
    }
    PushReturn();
    mPC = pc;
    mState = Execute(true);
    switch (mState) {
    default:
        mState = STATE_END;
        break;
    case STATE_WAIT:
        Pop(*pResult);
        break;
    case STATE_READY:
        break;
    }
    return true;
}

void CEngine::PushReturn() {
    mReturns[mReturnCount] = mPC;
    mReturns[mReturnCount++] = mPC;
}

void CEngine::Interrupt(const CValue& rValue) {
    mInterrupted = true;
    mInterruptValue = rValue;
}

CValue* CEngine::GetVariable(int baseIndex, int index) {
    if (baseIndex >= 0) {
        return &mStack[mBases[baseIndex] + index];
    }
    if (baseIndex == -1) {
        return &mEnv->mGlobals[index];
    }
}

void CEngine::UnaryOp(int op) {
    CValue& rValue = mStack[mSP - 1];
    int value;
    switch (rValue.mType) {
    case CValue::TYPE_REFERENCE:
        if (op == OP_NOT) {
            rValue.SetInt(rValue.IsNull());
        } else {
            rValue.SetReference(nullptr);
        }
        return;
    case CValue::TYPE_FUNCTION:
        rValue.SetFunction(0);
        return;
    case CValue::TYPE_FLOAT:
        {
            f32 floatValue = rValue.ToFloat();
            switch (op) {
            case OP_NEG:
                rValue.SetFloat(-floatValue);
                break;
            case OP_NOT:
                rValue.SetFloat(!floatValue);
                break;
            case OP_BITNOT:
                rValue.SetInt(0);
                break;
            }
        }
        return;
    case CValue::TYPE_STRING:
    case CValue::TYPE_STRING_PTR:
        value = ParseInt(rValue);
        break;
    case CValue::TYPE_WSTRING:
    case CValue::TYPE_WSTRING_PTR:
        value = ParseWInt(rValue);
        break;
    case CValue::TYPE_INT:
        value = rValue.ToInt();
        break;
    default:
        return;
    }
    switch (op) {
    case OP_NEG:
        rValue.SetInt(-value);
        break;
    case OP_NOT:
        rValue.SetInt(!value);
        break;
    case OP_BITNOT:
        rValue.SetInt(~value);
        break;
    }
}

void CEngine::BinaryOp(int op) {
    CValue operands[2];
    Pop(operands[1]);
    ConvertToNumber(operands[1]);
    Pop(operands[0]);
    ConvertToNumber(operands[0]);
    CValue& right = operands[1];
    CValue& left = operands[0];

    CValue result;
    bool rightIsReference = right.mType == CValue::TYPE_REFERENCE;
    bool leftIsReference = left.mType == CValue::TYPE_REFERENCE;
    if (rightIsReference || leftIsReference) {
        result.SetReference(nullptr);
        switch (op) {
        case OP_ADD:
            if (rightIsReference && leftIsReference) {
                break;
            }
            if (rightIsReference) {
                if (!right.IsNull()) {
                    result.SetReference(right.mReference + left.ToInt());
                }
            } else if (leftIsReference) {
                if (!left.IsNull()) {
                    result.SetReference(left.mReference + right.ToInt());
                }
            }
            break;
        case OP_SUB:
            if (leftIsReference && !left.IsNull() && !right.IsNull()) {
                if (rightIsReference) {
                    result.SetInt(left.mReference - right.mReference);
                } else if (right.IsInt()) {
                    result.SetReference(left.mReference - right.ToInt());
                }
            }
            break;
        }
        Push(result);
        return;
    }

    if (right.mType == CValue::TYPE_FLOAT || left.mType == CValue::TYPE_FLOAT) {
        f32 b = right.ToFloat();
        f32 a = left.ToFloat();
        f32 value;
        switch (op) {
        case OP_ADD:
            value = a + b;
            break;
        case OP_SUB:
            value = a - b;
            break;
        case OP_MUL:
            if (0.0f == a || 0.0f == b) {
                value = 0.0f;
            } else if (1.0f == a) {
                value = b;
            } else if (1.0f == b) {
                value = a;
            } else if (-1.0f == a) {
                value = -b;
            } else if (-1.0f == b) {
                value = -a;
            } else {
                value = a * b;
            }
            break;
        case OP_DIV:
            if (0.0f == a) {
                value = 0.0f;
            } else if (1.0f == b) {
                value = a;
            } else if (-1.0f == b) {
                value = -a;
            } else if (a == b) {
                value = 1.0f;
            } else if (a == -b) {
                value = -1.0f;
            } else {
                value = a / b;
            }
            break;
        case OP_MOD:
            value = 0.0f;
            break;
        }
        result.SetFloat(value);
    } else {
        int b = right.ToInt();
        int a = left.ToInt();
        int value;
        switch (op) {
        case OP_ADD:
            value = a + b;
            break;
        case OP_SUB:
            value = a - b;
            break;
        case OP_MUL:
            value = a * b;
            break;
        case OP_DIV:
            value = a / b;
            break;
        case OP_MOD:
            value = a % b;
            break;
        }
        result.SetInt(value);
    }
    Push(result);
}

void CEngine::BitwiseOp(int op) {
    CValue operands[2];
    Pop(operands[1]);
    ConvertToNumber(operands[1]);
    Pop(operands[0]);
    ConvertToNumber(operands[0]);
    CValue& right = operands[1];
    CValue& left = operands[0];

    if (right.mType == CValue::TYPE_REFERENCE || left.mType == CValue::TYPE_REFERENCE) {
        mStack[mSP++].SetReference(nullptr);
        return;
    }

    CValue result;
    if (right.mType == CValue::TYPE_FLOAT || left.mType == CValue::TYPE_FLOAT) {
        result.SetInt(0);
    } else {
        int value;
        int b = right.ToInt();
        int a = left.ToInt();
        switch (op) {
        case OP_AND:
            value = a & b;
            break;
        case OP_XOR:
            value = a ^ b;
            break;
        case OP_OR:
            value = a | b;
            break;
        case OP_SHR:
            value = a >> b;
            break;
        case OP_SHL:
            value = a << b;
            break;
        }
        result.SetInt(value);
    }
    Push(result);
}

void CEngine::CompareOp(int op) {
    CValue operands[2];
    Pop(operands[1]);
    ConvertToNumber(operands[1]);
    Pop(operands[0]);
    ConvertToNumber(operands[0]);
    CValue& right = operands[1];
    CValue& left = operands[0];

    int value = 0;
    bool rightIsReference = right.mType == CValue::TYPE_REFERENCE;
    bool leftIsReference = left.mType == CValue::TYPE_REFERENCE;
    if (rightIsReference && leftIsReference) {
        bool rightIsNull = right.mReference == nullptr;
        bool leftIsNull = left.mReference == nullptr;
        if (rightIsNull || leftIsNull) {
            switch (op) {
            case OP_EQ:
                value = rightIsNull && leftIsNull;
                break;
            case OP_NE:
                value = !rightIsNull || !leftIsNull;
                break;
            }
        } else {
            CValue* b = right.mReference;
            CValue* a = left.mReference;
            switch (op) {
            case OP_EQ:
                value = b == a;
                break;
            case OP_NE:
                value = b != a;
                break;
            case OP_GT:
                value = a > b;
                break;
            case OP_LT:
                value = a < b;
                break;
            case OP_GE:
                value = a >= b;
                break;
            case OP_LE:
                value = a <= b;
                break;
            }
        }
        PushInt(value);
        return;
    }
    if (rightIsReference || leftIsReference) {
        PushInt(0);
        return;
    }

    if (right.mType == CValue::TYPE_FLOAT || left.mType == CValue::TYPE_FLOAT) {
        f32 b = right.ToFloat();
        f32 a = left.ToFloat();
        switch (op) {
        case OP_EQ:
            value = b == a;
            break;
        case OP_NE:
            value = !(b == a);
            break;
        case OP_GT:
            value = a > b;
            break;
        case OP_LT:
            value = a < b;
            break;
        case OP_GE:
            value = a >= b;
            break;
        case OP_LE:
            value = a <= b;
            break;
        case OP_LAND:
            value = (int)b && (int)a;
            break;
        case OP_LOR:
            value = (int)b || (int)a;
            break;
        }
    } else {
        int b = right.ToInt();
        int a = left.ToInt();
        switch (op) {
        case OP_EQ:
            value = a == b;
            break;
        case OP_NE:
            value = a != b;
            break;
        case OP_GT:
            value = a > b;
            break;
        case OP_LT:
            value = a < b;
            break;
        case OP_GE:
            value = a >= b;
            break;
        case OP_LE:
            value = a <= b;
            break;
        case OP_LAND:
            value = b && a;
            break;
        case OP_LOR:
            value = b || a;
            break;
        }
    }
    PushInt(value);
}

int CEngine::Execute(int isCall) {
    u32 code;
    int op;
    int returnCount;
    int modifier;
    if (isCall) {
        returnCount = mReturnCount;
    }
    if (!isCall && mInterrupted) {
        mInterrupted = false;
        mThrown = mInterruptValue;
        if (!Unwind()) {
            mPC = 0;
            mSP = 0;
            mFrameCount = 0;
            mReturnCount = 0;
            return STATE_READY;
        }
    }

    modifier = OP_NONE;
    for (;;) {
        code = mCode->mCode[mPC++];
        op = static_cast<int>(code) >> 24;
        if (op == OP_LINE) {
            OnLineBegin();
            mPC += 2;
            code = mCode->mCode[mPC++];
            op = static_cast<int>(code) >> 24;
            OnLineEnd();
        }

        switch (op) {
        case OP_NOP:
        case OP_NONE:
        case OP_NOP_2:
            break;
        case OP_DUP:
            {
                CValue value;
                GetArg(0, value);
                Push(value);
            }
            break;
        case OP_POP:
            {
                CValue value;
                Pop(value);
            }
            break;
        case OP_NEG:
        case OP_BITNOT:
        case OP_NOT:
            UnaryOp(op);
            break;
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
            BinaryOp(op);
            break;
        case OP_EQ:
        case OP_NE:
        case OP_GT:
        case OP_LT:
        case OP_GE:
        case OP_LE:
        case OP_LAND:
        case OP_LOR:
            CompareOp(op);
            break;
        case OP_AND:
        case OP_XOR:
        case OP_OR:
        case OP_SHR:
        case OP_SHL:
            BitwiseOp(op);
            break;
        case OP_PUSH_INT:
            PushInt(Fetch());
            break;
        case OP_PUSH_SHORT:
            PushInt(static_cast<int>(code << 8) >> 8);
            break;
        case OP_PUSH_FLOAT:
            PushFloat(*reinterpret_cast<const f32*>(&mCode->mCode[mPC++]));
            break;
        case OP_PUSH_STRING:
            {
                CValue value;
                value.SetString(code & 0xFFFFFF);
                Push(value);
            }
            break;
        case OP_PUSH_WSTRING:
            {
                CValue value;
                value.SetWString(code & 0xFFFFFF);
                Push(value);
            }
            break;
        case OP_PUSH_FUNCTION:
            {
                CValue value;
                value.SetFunction(code & 0xFFFFFF);
                Push(value);
            }
            break;
        case OP_PRE_INC:
        case OP_POST_INC:
        case OP_PRE_DEC:
        case OP_POST_DEC:
            modifier = op;
            break;
        case OP_LOAD:
            {
                CValue* pValue = GetVariable(static_cast<s8>((code >> 16) & 0xFF), code & 0xFFFF);
                if (modifier == OP_PRE_INC) {
                    pValue->Increment();
                }
                if (modifier == OP_PRE_DEC) {
                    pValue->Decrement();
                }
                Push(*pValue);
                if (modifier == OP_POST_INC) {
                    pValue->Increment();
                }
                if (modifier == OP_POST_DEC) {
                    pValue->Decrement();
                }
                modifier = OP_NONE;
            }
            break;
        case OP_STORE:
            Pop(*GetVariable(static_cast<s8>((code >> 16) & 0xFF), code & 0xFFFF));
            break;
        case OP_LOAD_ELEMENT:
            {
                CValue* pValue = GetVariable(static_cast<s8>((code >> 16) & 0xFF), code & 0xFFFF);
                pValue += PopInt();
                if (modifier == OP_PRE_INC) {
                    pValue->Increment();
                }
                if (modifier == OP_PRE_DEC) {
                    pValue->Decrement();
                }
                Push(*pValue);
                if (modifier == OP_POST_INC) {
                    pValue->Increment();
                }
                if (modifier == OP_POST_DEC) {
                    pValue->Decrement();
                }
                modifier = OP_NONE;
            }
            break;
        case OP_LOAD_INDIRECT:
            {
                CValue reference = *GetVariable(static_cast<s8>((code >> 16) & 0xFF), code & 0xFFFF);
                CValue* pValue = reference.GetReference();
                pValue += PopInt();
                if (modifier == OP_PRE_INC) {
                    pValue->Increment();
                }
                if (modifier == OP_PRE_DEC) {
                    pValue->Decrement();
                }
                Push(*pValue);
                if (modifier == OP_POST_INC) {
                    pValue->Increment();
                }
                if (modifier == OP_POST_DEC) {
                    pValue->Decrement();
                }
                modifier = OP_NONE;
            }
            break;
        case OP_ADDRESS:
        case OP_ADDRESS_ELEMENT:
            {
                CValue* pValue = GetVariable(static_cast<s8>((code >> 16) & 0xFF), code & 0xFFFF);
                if (op == OP_ADDRESS_ELEMENT) {
                    pValue += PopInt();
                }
                CValue value;
                value.SetReference(pValue);
                Push(value);
            }
            break;
        case OP_ADDRESS_INDIRECT:
            {
                CValue value = *GetVariable(static_cast<s8>((code >> 16) & 0xFF), code & 0xFFFF);
                CValue* pValue = value.GetReference();
                value.SetReference(pValue + PopInt());
                Push(value);
            }
            break;
        case OP_STORE_ELEMENT:
            {
                CValue* pValue = GetVariable(static_cast<s8>((code >> 16) & 0xFF), code & 0xFFFF);
                CValue value;
                Pop(value);
                pValue[PopInt()] = value;
            }
            break;
        case OP_STORE_INDIRECT:
            {
                CValue reference = *GetVariable(static_cast<s8>((code >> 16) & 0xFF), code & 0xFFFF);
                CValue* pValue = reference.GetReference();
                CValue value;
                Pop(value);
                pValue[PopInt()] = value;
            }
            break;
        case OP_JUMP:
            mPC = code & 0xFFFFFF;
            break;
        case OP_JUMP_FALSE:
        case OP_JUMP_TRUE:
        case OP_JUMP_FALSE_KEEP:
        case OP_JUMP_TRUE_KEEP:
            {
                CValue value;
                Pop(value);
                bool condition = value.ToBool();
                if (op == OP_JUMP_FALSE || op == OP_JUMP_FALSE_KEEP) {
                    condition = !condition;
                }
                if (condition) {
                    if (op == OP_JUMP_FALSE_KEEP || op == OP_JUMP_TRUE_KEEP) {
                        Push(value);
                    }
                    mPC = code & 0xFFFFFF;
                }
            }
            break;
        case OP_CALL_NATIVE:
        case OP_CALL_NATIVE_2:
            {
                OnLineBegin();
                const SNativeFunction* pNative = mEnv->GetNative(code & 0xFFFFFF);
                int result = pNative->mFunc(this);
                if (mState != STATE_IDLE && mState != STATE_WAIT) {
                    result = NATIVE_STOP;
                }
                if (result == NATIVE_YIELD || result == NATIVE_WAIT) {
                    return STATE_WAIT;
                }
                if (result == NATIVE_CONTINUE) {
                    break;
                }
            }
        case OP_END:
            mPC = 0;
            mSP = 0;
            mFrameCount = 0;
            mReturnCount = 0;
            return mState == STATE_END ? STATE_END : STATE_READY;
        case OP_CALL_INDIRECT:
            {
                int argCount = ((code >> 16) & 0xFF) - 1;
                PushReturn();
                mSP -= argCount;
                mPC = PopInt();
                for (int i = 0; i < argCount; i++) {
                    Push(mStack[mSP + 1]);
                }
            }
            break;
        case OP_CALL:
            PushReturn();
            mPC = code & 0xFFFFFF;
            break;
        case OP_RETURN:
            {
                CValue value;
                Pop(value);
                PopFrame();
                Push(value);
                if (isCall && mReturnCount < returnCount) {
                    return STATE_WAIT;
                }
            }
            break;
        case OP_ENTER:
            {
                u32 info = Fetch();
                int baseIndex = static_cast<int>(info) >> 24;
                int argCount = (info >> 16) & 0xFF;
                int localCount = info & 0xFFFF;
                mSP -= argCount;
                PushFrame(baseIndex);
                memset(&mStack[mSP + argCount], 0, (localCount - argCount) * sizeof(CValue));
                mBases[baseIndex] = mSP;
                mSP += localCount;
            }
            break;
        case OP_PUSH_THIS:
            Push(mThis);
            break;
        case OP_CASE:
            {
                int value = Fetch();
                int target = code & 0xFFFFFF;
                int arg = GetArgInt(0);
                if (arg == value) {
                    mPC = target;
                    Drop(1);
                }
            }
            break;
        case OP_IN_RANGE:
            {
                CValue lower;
                CValue upper;
                CValue value;
                Pop(upper);
                Pop(lower);
                Pop(value);
                if (value.IsString()) {
                    value.SetInt(ParseInt(value));
                }
                if (lower.IsString()) {
                    lower.SetInt(ParseInt(lower));
                }
                if (upper.IsString()) {
                    upper.SetInt(ParseInt(upper));
                }
                if (value.IsWString()) {
                    value.SetInt(ParseWInt(value));
                }
                if (lower.IsWString()) {
                    lower.SetInt(ParseWInt(lower));
                }
                if (upper.IsWString()) {
                    upper.SetInt(ParseWInt(upper));
                }
                if (value.IsFloat() || lower.IsFloat() || upper.IsFloat()) {
                    f32 upperFloat = upper.ToFloat();
                    f32 lowerFloat = lower.ToFloat();
                    f32 valueFloat = value.ToFloat();
                    PushInt((lowerFloat <= valueFloat && valueFloat <= upperFloat) ||
                            (upperFloat <= valueFloat && valueFloat <= lowerFloat));
                } else {
                    int upperInt = upper.ToInt();
                    int lowerInt = lower.ToInt();
                    int valueInt = value.ToInt();
                    PushInt((lowerInt <= valueInt && valueInt <= upperInt) || (upperInt <= valueInt && valueInt <= lowerInt));
                }
            }
            break;
        case OP_SELECT:
            {
                CValue trueValue;
                CValue falseValue;
                Pop(falseValue);
                Pop(trueValue);
                if (PopInt()) {
                    Push(trueValue);
                } else {
                    Push(falseValue);
                }
            }
            break;
        case OP_DEREF:
            {
                CValue value;
                Pop(value);
                if (value.IsNull()) {
                    Push(value);
                } else {
                    CValue* pValue = value.GetReference();
                    if (mStack <= pValue && pValue < mStack + mSP) {
                        Push(*pValue);
                    } else if (mEnv->mGlobals <= pValue && pValue < mEnv->mGlobals + mEnv->mGlobalCount) {
                        Push(*pValue);
                    } else {
                        PushInt(0);
                    }
                }
            }
            break;
        case OP_STORE_DEREF:
            {
                CValue reference;
                CValue value;
                Pop(value);
                Pop(reference);
                if (!reference.IsNull()) {
                    CValue* pValue = reference.GetReference();
                    if (mStack <= pValue && pValue < mStack + mSP) {
                        *pValue = value;
                    } else if (mEnv->mGlobals <= pValue && pValue < mEnv->mGlobals + mEnv->mGlobalCount) {
                        *pValue = value;
                    }
                }
            }
            break;
        case OP_PUSH_NULL:
            {
                CValue value;
                value.SetReference(nullptr);
                Push(value);
            }
            break;
        case OP_CHECK_ARG:
            if (GetArgInt(0) < 0) {
                break;
            }
            break;
        case OP_INDEX_2:
            {
                int size = Fetch();
                int index = PopInt();
                int row = PopInt();
                PushInt(index + row * size);
            }
            break;
        case OP_INDEX_3:
            {
                u32 sizes = Fetch();
                int index0 = PopInt();
                int index1 = PopInt();
                int index2 = PopInt();
                PushInt(index0 + (sizes >> 16) * (index1 + index2 * (sizes & 0xFFFF)));
            }
            break;
        case OP_INDEX_4:
            {
                u32 sizes = Fetch();
                int size = Fetch();
                int index0 = PopInt();
                int index1 = PopInt();
                int index2 = PopInt();
                int index3 = PopInt();
                PushInt(index0 + size * (index1 + (sizes >> 16) * (index2 + index3 * (sizes & 0xFFFF))));
            }
            break;
        case OP_TYPEOF:
            {
                CValue value;
                Pop(value);
                PushInt(value.mType);
            }
            break;
        case OP_SWAP:
            {
                CValue first;
                Pop(first);
                CValue second;
                Pop(second);
                Push(first);
                Push(second);
            }
            break;
        case OP_TRY:
            {
                SHandler& rHandler = mHandlers[mHandlerCount++];
                rHandler.mPC = mPC;
                rHandler.mSP = mSP;
                rHandler.mFrameCount = mFrameCount;
                PushInt(1);
            }
            break;
        case OP_END_TRY:
            mHandlerCount--;
            break;
        case OP_PUSH_THROWN:
            Push(mThrown);
            break;
        case OP_POP_THROWN:
            Pop(mThrown);
            break;
        case OP_THROW:
            if (!Unwind()) {
                mReturnCount = 0;
                mFrameCount = 0;
                mSP = 0;
                mPC = 0;
                return mState == STATE_END ? STATE_END : STATE_READY;
            }
            break;
        case OP_SWITCH:
            {
                int base = mPC - 1;
                int count = (code >> 16) & 0xFF;
                int defaultOffset = static_cast<s16>(code & 0xFFFF);
                int value = PopInt();
                for (int i = 0; i < count; i++) {
                    u32 entry = Fetch();
                    if (value == static_cast<int>(entry) >> 16) {
                        mPC = base + static_cast<s16>(entry);
                        goto next;
                    }
                }
                mPC = base + defaultOffset;
            }
            break;
        case OP_TABLE_SWITCH:
            {
                int count = (code >> 16) & 0xFF;
                int base = mPC - 1;
                int defaultOffset = static_cast<s16>(code & 0xFFFF);
                int value = PopInt();
                if (value >= 0 && value < count) {
                    u32 entry = mCode->mCode[mPC + value / 2];
                    int offset;
                    if (value & 1) {
                        offset = static_cast<int>(entry) >> 16;
                    } else {
                        offset = static_cast<s16>(entry);
                    }
                    mPC = base + offset;
                } else {
                    mPC = base + defaultOffset;
                }
            }
            break;
        default:
            return STATE_END;
        }
    next:;
    }
}

bool CEngine::Unwind() {
    while (mHandlerCount > 0) {
        SHandler& rHandler = mHandlers[mHandlerCount - 1];
        if (rHandler.mFrameCount == mFrameCount) {
            mHandlerCount--;
            mPC = rHandler.mPC;
            mSP = rHandler.mSP;
            PushInt(0);
            return true;
        }
        PopFrame();
    }
    return false;
}

void CEngine::SetID(int id) {
    mID = id;
}

void CEngine::SetNextID(int id) {
    sNextID = id;
}

const char* CEngine::ToString(const CValue& rValue) const {
    static char sBuffer[32];

    if (rValue.mType == CValue::TYPE_STRING) {
        return mCode->mStrings[rValue.mIndex];
    }
    if (rValue.mType == CValue::TYPE_STRING_PTR) {
        return rValue.mString;
    }
    if (rValue.IsInt()) {
        snprintf(sBuffer, sizeof(sBuffer), "%d", rValue.ToInt());
    } else if (rValue.mType == CValue::TYPE_FLOAT) {
        snprintf(sBuffer, sizeof(sBuffer), "%g", rValue.ToFloat());
    } else {
        sBuffer[0] = '\0';
    }
    return sBuffer;
}

const wchar_t* CEngine::ToWString(const CValue& rValue) const {
    static wchar_t sBuffer[32];

    if (rValue.mType == CValue::TYPE_WSTRING) {
        return mCode->mWStrings[rValue.mIndex];
    }
    if (rValue.mType == CValue::TYPE_WSTRING_PTR) {
        return rValue.mWString;
    }
    if (rValue.IsInt()) {
        swprintf(sBuffer, 32, L"%d", rValue.ToInt());
    } else if (rValue.mType == CValue::TYPE_FLOAT) {
        swprintf(sBuffer, 32, L"%g", rValue.ToFloat());
    } else {
        sBuffer[0] = L'\0';
    }
    return sBuffer;
}

inline std::wstring ToUnicode(const std::string& rString) {
    s32 size;
    ENCConvertStringSjisToUnicode(nullptr, &size, reinterpret_cast<const u8*>(rString.c_str()), nullptr);
    wchar_t* pBuffer = static_cast<wchar_t*>(__alloca((size + 1) * sizeof(wchar_t)));
    pBuffer[size] = L'\0';
    ENCConvertStringSjisToUnicode(reinterpret_cast<u16*>(pBuffer), &size, reinterpret_cast<const u8*>(rString.c_str()), nullptr);
    return std::wstring(pBuffer);
}

inline std::string ToSjis(const std::wstring& rString) {
    s32 size;
    ENCConvertStringUnicodeToSjis(nullptr, &size, reinterpret_cast<const u16*>(rString.c_str()), nullptr);
    char* pBuffer = static_cast<char*>(__alloca(size + 1));
    pBuffer[size] = '\0';
    ENCConvertStringUnicodeToSjis(reinterpret_cast<u8*>(pBuffer), &size, reinterpret_cast<const u16*>(rString.c_str()), nullptr);
    return std::string(pBuffer);
}

std::string CEngine::ToStdString(const CValue& rValue) const {
    if (rValue.IsString()) {
        return std::string(ToString(rValue));
    }
    if (rValue.IsWString()) {
        std::wstring string(ToWString(rValue));
        return ToSjis(string);
    }
    return std::string("");
}

std::wstring CEngine::ToStdWString(const CValue& rValue) const {
    if (rValue.IsWString()) {
        return std::wstring(ToWString(rValue));
    }
    if (rValue.IsString()) {
        std::string string(ToString(rValue));
        return ToUnicode(string);
    }
    return std::wstring(L"");
}

void CEngine::GetThrown(CValue& rValue) const {
    rValue = mThrown;
}

const char* CEngine::AllocString(const std::string& rString) {
    sGCAllocCount++;
    SGCString<char>* pString = new SGCString<char>;
    pString->mString = rString;
    pString->mMark = sGCMark;
    sStrings->insert(SGCStringRef<char>(pString->mString.c_str(), pString));
    return pString->mString.c_str();
}

const wchar_t* CEngine::AllocWString(const std::wstring& rString) {
    sGCAllocCount++;
    SGCString<wchar_t>* pString = new SGCString<wchar_t>;
    pString->mString = rString;
    pString->mMark = sGCMark;
    sWStrings->insert(SGCStringRef<wchar_t>(pString->mString.c_str(), pString));
    return pString->mString.c_str();
}

}
