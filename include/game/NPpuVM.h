#ifndef GAME_NPPUVM_H
#define GAME_NPPUVM_H

#include <revolution/types.h>
#include <set>

namespace NPpu {

class CVM;

typedef int (*NativeFunc)(CVM* pVM);

struct SNativeFunction {
    NativeFunc mFunc;
    const char* mName;
    int mArgCount;
};

class CValue {
public:
    enum Type {
        TYPE_INT = 0,
        TYPE_FLOAT = 1,
        TYPE_STRING = 2,
        TYPE_STRING_PTR = 3,
        TYPE_REFERENCE = 4,
        TYPE_FUNCTION = 5,
        TYPE_WSTRING = 6,
        TYPE_WSTRING_PTR = 7
    };

    bool IsInt() const { return mType == TYPE_INT || mType == TYPE_FUNCTION; }
    bool IsFloat() const;
    bool IsNull() const;
    bool IsString() const { return mType == TYPE_STRING || mType == TYPE_STRING_PTR; }
    bool IsWString() const { return mType == TYPE_WSTRING || mType == TYPE_WSTRING_PTR; }

    int ToInt() const;
    bool ToBool() const;
    f32 ToFloat() const;
    CValue* GetReference() const;

    void SetReference(CValue* pValue);
    void Increment();
    void Decrement();

    int mType;
    union {
        int mInt;
        f32 mFloat;
        u32 mIndex;
        const char* mString;
        const wchar_t* mWString;
        CValue* mReference;
    };
};

class CCode {
public:
    enum {
        MAGIC = 0x706B1A20,
        MAGIC_OLD = 0x201A706B,
        MAGIC_SWAPPED = 0x201A6B70,
        MAGIC_OLD_SWAPPED = 0x6B701A20
    };

    struct SHeader {
        u32 mMagic;
        int mStackSize;
        int mCodeSize;
        int mWStringDataSize;
        int mWStringCount;
        int mStringDataSize;
        int mStringCount;
        u32 mReserved;
    };

private:
    const char* mName;
    int mStackSize;
    int mCodeSize;
    int mStringCount;
    int mWStringCount;
    const u32* mCode;
    const char** mStrings;
    const wchar_t** mWStrings;

public:
    CCode();
    virtual ~CCode();
    virtual void AddRef() {}
    virtual void Release() {}

    bool Load(const char* pName, const void* pData, u32 size, int, int);

    int GetStackSize() const { return mStackSize; }
    const char* GetString(int index) const {
        if (mStrings != nullptr && index >= 0 && index < mStringCount) {
            return mStrings[index];
        }
        return nullptr;
    }
    const wchar_t* GetWString(int index) const {
        if (mWStrings != nullptr && index >= 0 && index < mWStringCount) {
            return mWStrings[index];
        }
        return nullptr;
    }

private:
    void Clear();
};

class CEnvironment {
public:
private:
    int mGlobalCount;
    CValue* mGlobals;
    u32 mNativeCount;
    const SNativeFunction* mNatives;

public:
    CEnvironment(int globalCount, const SNativeFunction* pNatives, u32 nativeCount);
    virtual ~CEnvironment();
    virtual void AddRef() {}
    virtual void Release() {}

    const SNativeFunction* GetNative(int index) const;

private:
    void AllocGlobals();
};

class CVM {
public:
    struct SFrame {
        int mSP;
        int mBaseIndex;
        int mSavedBase;
    };

    struct SHandler {
        int mPC;
        int mSP;
        int mFrameCount;
    };

    enum State {
        STATE_NONE = 0,
        STATE_IDLE = 1,
        STATE_WAIT = 2,
        STATE_READY = 3,
        STATE_END = 4
    };

    CVM();
    ~CVM();

    void Init(CCode* pCode, CEnvironment* pEnv, int frameCapacity, int baseCount, int handlerCapacity);

    int ParseInt(const CValue& rValue) const;
    int ParseWInt(const CValue& rValue) const;

    void SetThis(const CValue& rValue);
    int Run();
    void SetID(int id);
    static void SetNextID(int id);

    CEnvironment* GetEnvironment() const { return mEnv; }
    State GetState() const { return static_cast<State>(mState); }
    CValue Pop() { return mStack[--mSP]; }

private:
    static int sNextID;
    static std::set<char*>* sStrings;
    static std::set<wchar_t*>* sWStrings;
    static std::set<CVM*>* sInstances;

    int mID;
    CCode* mCode;
    CEnvironment* mEnv;
    int mFrameCapacity;
    int mBaseCount;
    int mHandlerCapacity;
    void* mBuffer;
    int mState;
    int mPC;
    int mSP;
    CValue* mStack;
    int mFrameCount;
    SFrame* mFrames;
    int mReturnCount;
    int* mReturns;
    int* mBases;
    int mHandlerCount;
    SHandler* mHandlers;
    CValue mThis;
    CValue mThrown;
    bool mInterrupted;
    CValue mInterruptValue;
};

}

#endif
