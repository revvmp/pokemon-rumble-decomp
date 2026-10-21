#ifndef GAME_NPPUVM_H
#define GAME_NPPUVM_H

#include <revolution/types.h>
#include <set>
#include <map>
#include <string>

namespace NPpu {

class CEngine;

typedef int (*NativeFunc)(CEngine* pEngine);

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

    void SetInt(int value) {
        mType = TYPE_INT;
        mInt = value;
    }
    void SetFloat(f32 value) {
        mType = TYPE_FLOAT;
        *reinterpret_cast<f32*>(&mInt) = value;
    }
    void SetString(u32 index) {
        mType = TYPE_STRING;
        mInt = index;
    }
    void SetWString(u32 index) {
        mType = TYPE_WSTRING;
        mInt = index;
    }
    void SetFunction(int value) {
        mType = TYPE_FUNCTION;
        mInt = value;
    }
    void SetStringPtr(const char* pString) {
        mType = TYPE_STRING_PTR;
        mInt = reinterpret_cast<int>(pString);
    }
    void SetWStringPtr(const wchar_t* pString) {
        mType = TYPE_WSTRING_PTR;
        mInt = reinterpret_cast<int>(pString);
    }
    void SetReference(CValue* pValue);
    void Increment();
    void Decrement();

    f32 GetFloat() const { return *reinterpret_cast<const f32*>(&mInt); }
    void SetFloatBits(f32 value) { *reinterpret_cast<f32*>(&mInt) = value; }
    const char* GetStringPtr() const { return reinterpret_cast<const char*>(mInt); }
    const wchar_t* GetWStringPtr() const { return reinterpret_cast<const wchar_t*>(mInt); }
    CValue* GetPointer() const { return reinterpret_cast<CValue*>(mInt); }
    void SetPointer(CValue* pValue) { mInt = reinterpret_cast<int>(pValue); }

    int mType;
    int mInt;
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

    friend class CEngine;

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

private:
    void Clear();
};

class CEnvironment {
    friend class CEngine;

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

template <typename T> struct SGCString {
    std::basic_string<T> mString;
    int mMark;
};

class CEngine {
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

    enum NativeResult {
        NATIVE_CONTINUE = 0,
        NATIVE_YIELD = 1,
        NATIVE_WAIT = 2,
        NATIVE_STOP = 3
    };

    CEngine();
    ~CEngine();

    void Init(CCode* pCode, CEnvironment* pEnv, int frameCapacity, int baseCount, int handlerCapacity);

    int ParseInt(const CValue& rValue) const;
    int ParseWInt(const CValue& rValue) const;

    void SetThis(const CValue& rValue);
    void GetThis(CValue& rValue) const;
    CValue* PopArray(int* pCount);
    bool IsIntArg() const;
    bool IsStringArg() const;
    void GetArg(int index, CValue& rValue) const;
    int GetArgInt(int index) const;
    int Run();
    void SetID(int id);
    static void SetNextID(int id);
    bool Reset();
    bool Call(CValue* pResult, int pc, int argCount, const CValue* pArgs);
    void Interrupt(const CValue& rValue);
    const char* ToString(const CValue& rValue) const;
    const wchar_t* ToWString(const CValue& rValue) const;
    std::string ToStdString(const CValue& rValue) const;
    std::wstring ToStdWString(const CValue& rValue) const;
    void GetThrown(CValue& rValue) const;
    const char* AllocString(const std::string& rString);
    const wchar_t* AllocWString(const std::wstring& rString);
    static void CollectGarbage(int threshold, int);

    CEnvironment* GetEnvironment() const { return mEnv; }
    State GetState() const { return static_cast<State>(mState); }

    void Push(const CValue& rValue) { mStack[mSP++] = rValue; }
    void Pop(CValue& rValue) { rValue = mStack[--mSP]; }
    void PushInt(int value) {
        CValue v;
        v.SetInt(value);
        Push(v);
    }
    void PushFloat(f32 value) {
        CValue v;
        v.SetFloat(value);
        Push(v);
    }
    void PushString(const char* pString) {
        CValue v;
        v.SetStringPtr(pString);
        Push(v);
    }
    void PushWString(const wchar_t* pString) {
        CValue v;
        v.SetWStringPtr(pString);
        Push(v);
    }
    int PopInt() {
        CValue value;
        Pop(value);
        return value.ToInt();
    }
    f32 PopFloat() {
        CValue value;
        Pop(value);
        return value.ToFloat();
    }
    CValue* PopReference() {
        CValue value;
        Pop(value);
        return value.GetReference();
    }
    void Drop(int count) { mSP -= count; }
    int FetchArgCount() { return Fetch(); }
    void PushNull() { PushInt(0); }

    void ConvertToNumber(CValue& rValue) {
        if (rValue.IsString()) {
            rValue.SetInt(ParseInt(rValue));
        }
        if (rValue.IsWString()) {
            rValue.SetInt(ParseWInt(rValue));
        }
    }

    u32 Fetch() { return mCode->mCode[mPC++]; }

private:
    void OnLineBegin() {}
    void OnLineEnd() {}
    void PushFrame(int baseIndex);
    void PopFrame();
    void PushReturn();
    CValue* GetVariable(int baseIndex, int index);
    void UnaryOp(int op);
    void BinaryOp(int op);
    void BitwiseOp(int op);
    void CompareOp(int op);
    int Execute(int isCall);
    bool Unwind();

private:
    static int sNextID;
    static int sGCMark;
    static int sGCAllocCount;
    static std::map<const char*, SGCString<char>* >* sStrings;
    static std::map<const wchar_t*, SGCString<wchar_t>* >* sWStrings;
    static std::set<CEngine*>* sInstances;

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
