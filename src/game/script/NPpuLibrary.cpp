#include <game/script/NPpuVM.h>

#include <math.h>
#include <revolution/os.h>
#include <revolution/os/OSAlloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wstring.h>

using namespace NPpu;

int ScriptBinding_GetThrownValue(CEngine* pEngine) {
    CValue value;
    pEngine->GetThrown(value);
    pEngine->Push(value);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Interrupt(CEngine* pEngine) {
    CValue value;
    pEngine->Pop(value);
    pEngine->Interrupt(value);
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Yield(CEngine* pEngine) {
    pEngine->PushInt(0);
    return CEngine::NATIVE_YIELD;
}

int ScriptBinding_Exit(CEngine* pEngine) {
    pEngine->PopInt();
    pEngine->PushInt(0);
    return CEngine::NATIVE_STOP;
}

int ScriptBinding_GetThisObject(CEngine* pEngine) {
    CValue value;
    pEngine->GetThis(value);
    pEngine->Push(value);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_ShowPPU(CEngine* pEngine) {
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_SetDebugLevel(CEngine* pEngine) {
    pEngine->PopInt();
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_SetInitialDebugLevel(CEngine* pEngine) {
    pEngine->PopInt();
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

std::wstring Format(CEngine* pEngine, int argCount, const wchar_t* pFormat) {
    wchar_t format[1024];
    wchar_t buffer[1024];
    int arg = 1 - argCount;
    wchar_t* pOut = buffer;
    wchar_t* pLimit = buffer + 1024;
    wchar_t* pSpec;
    int size;
    wchar_t* pIn = format;
    wchar_t type;
    wcsncpy(format, pFormat, 1024);
    format[1023] = L'\0';

    while (pOut < pLimit - 1 && *pIn != L'\0' && arg < 0) {
        if (*pIn != L'%') {
            *pOut++ = *pIn++;
            continue;
        }

        pSpec = pIn;
        wchar_t* pEnd = pIn + 1;
        size = 0;
        for (;;) {
            type = *pEnd;
            if (type == L'\0') {
                goto end;
            }
            if (wcschr(L"hjlLqtz", type) != nullptr) {
                size = type;
            }
            if (wcschr(L"%cCsSdiouxXfeEgGnp", type) != nullptr) {
                break;
            }
            pEnd++;
        }

        pIn = pEnd + 1;
        wchar_t next = *pIn;
        int capacity = pLimit - pOut;
        *pIn = L'\0';

        CValue value;
        switch (type) {
        case L'%':
            swprintf(pOut, capacity, pSpec);
            break;
        case L'c':
        case L'C':
        case L'd':
        case L'i':
        case L'o':
        case L'u':
        case L'x':
        case L'X':
            pEngine->GetArg(-1 - arg++, value);
            swprintf(pOut, capacity, pSpec, value.ToInt());
            break;
        case L'e':
        case L'E':
        case L'f':
        case L'g':
        case L'G':
            pEngine->GetArg(-1 - arg++, value);
            swprintf(pOut, capacity, pSpec, value.ToFloat());
            break;
        case L'S':
            size = L'l';
        case L's':
            pEngine->GetArg(-1 - arg++, value);
            if (size == L'l') {
                swprintf(pOut, capacity, pSpec, pEngine->ToWString(value));
            } else {
                swprintf(pOut, capacity, pSpec, pEngine->ToString(value));
            }
            break;
        case L'p':
            pEngine->GetArg(-1 - arg++, value);
            swprintf(pOut, capacity, pSpec, value.GetReference());
            break;
        default:
            pIn = pSpec;
            goto end;
        }

        pOut += wcslen(pOut);
        *pIn = next;
    }

end:
    while (pOut < pLimit - 1 && *pIn != L'\0') {
        if (*pIn == L'%' && pIn[1] == L'%') {
            pIn++;
        }
        *pOut++ = *pIn++;
    }
    *pOut = L'\0';
    return std::wstring(buffer);
}

std::string Format(CEngine* pEngine, int argCount, const char* pFormat) {
    char format[1024];
    char buffer[1024];
    char type;
    int arg = 1 - argCount;
    char* pOut = buffer;
    char* pLimit = buffer + 1024;
    char* pIn = format;
    strncpy(format, pFormat, 1024);
    format[1023] = '\0';

    while (pOut < pLimit - 1 && *pIn != '\0' && arg < 0) {
        if (*pIn != '%') {
            *pOut++ = *pIn++;
            continue;
        }

        char* pSpec = pIn;
        char* pEnd = pIn + 1;
        int size = 0;
        for (;;) {
            type = *pEnd;
            if (type == '\0') {
                goto end;
            }
            if (strchr("hjlLqtz", type) != nullptr) {
                size = type;
            }
            if (strchr("%cCsSdiouxXfeEgGnp", type) != nullptr) {
                break;
            }
            pEnd++;
        }

        pIn = pEnd + 1;
        char next = *pIn;
        int capacity = pLimit - pOut;
        *pIn = '\0';

        CValue value;
        switch (type) {
        case '%':
            snprintf(pOut, capacity, pSpec);
            break;
        case 'c':
        case 'C':
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
            pEngine->GetArg(-1 - arg++, value);
            snprintf(pOut, capacity, pSpec, value.ToInt());
            break;
        case 'e':
        case 'E':
        case 'f':
        case 'g':
        case 'G':
            pEngine->GetArg(-1 - arg++, value);
            snprintf(pOut, capacity, pSpec, value.ToFloat());
            break;
        case 'S':
            size = 'l';
        case 's':
            pEngine->GetArg(-1 - arg++, value);
            if (size == 'l') {
                snprintf(pOut, capacity, pSpec, pEngine->ToWString(value));
            } else {
                snprintf(pOut, capacity, pSpec, pEngine->ToString(value));
            }
            break;
        case 'p':
            pEngine->GetArg(-1 - arg++, value);
            snprintf(pOut, capacity, pSpec, value.GetReference());
            break;
        default:
            pIn = pSpec;
            goto end;
        }

        pOut += strlen(pOut);
        *pIn = next;
    }

end:
    while (pOut < pLimit - 1 && *pIn != '\0') {
        if (*pIn == '%' && pIn[1] == '%') {
            pIn++;
        }
        *pOut++ = *pIn++;
    }
    *pOut = '\0';
    return std::string(buffer);
}

int ScriptBinding_Printf(CEngine* pEngine) {
    int argCount = pEngine->Fetch();
    if (argCount > 0) {
        CValue format;
        pEngine->GetArg(argCount - 1, format);
        if (format.IsWString()) {
            Format(pEngine, argCount, pEngine->ToWString(format));
        } else if (format.IsString()) {
            Format(pEngine, argCount, pEngine->ToString(format));
        }
        pEngine->Drop(argCount);
    }
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Sprintf(CEngine* pEngine) {
    int argCount = pEngine->Fetch();
    if (argCount == 0) {
        pEngine->PushString("");
    } else {
        CValue format;
        pEngine->GetArg(argCount - 1, format);
        if (format.IsWString()) {
            std::wstring result = Format(pEngine, argCount, pEngine->ToWString(format));
            pEngine->Drop(argCount);
            pEngine->PushWString(pEngine->AllocWString(result));
        } else if (format.IsString()) {
            std::string result = Format(pEngine, argCount, pEngine->ToString(format));
            pEngine->Drop(argCount);
            pEngine->PushString(pEngine->AllocString(result));
        } else {
            pEngine->Drop(argCount);
            pEngine->PushString("");
        }
    }
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Sin(CEngine* pEngine) {
    pEngine->PushFloat(sin(pEngine->PopFloat()));
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Cos(CEngine* pEngine) {
    pEngine->PushFloat(cos(pEngine->PopFloat()));
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Atan(CEngine* pEngine) {
    pEngine->PushFloat(atan(pEngine->PopFloat()));
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Log(CEngine* pEngine) {
    pEngine->PushFloat(log(pEngine->PopFloat()));
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Exp(CEngine* pEngine) {
    pEngine->PushFloat(exp(pEngine->PopFloat()));
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding__Sqrt(CEngine* pEngine) {
    pEngine->PushFloat(sqrt(pEngine->PopFloat()));
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_CreateTimer(CEngine* pEngine) {
    OSTime* pTimer = static_cast<OSTime*>(OSAlloc(sizeof(OSTime)));
    pEngine->PushInt(reinterpret_cast<int>(pTimer));
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_ResetTimer(CEngine* pEngine) {
    OSTime* pTimer = reinterpret_cast<OSTime*>(pEngine->PopInt());
    *pTimer = OSGetTime();
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_GetTime(CEngine* pEngine) {
    OSTime* pTimer = reinterpret_cast<OSTime*>(pEngine->PopInt());
    OSTime elapsed = OSGetTime() - *pTimer;
    pEngine->PushInt(OSTicksToMicroseconds(elapsed));
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_GetPlatform(CEngine* pEngine) {
    pEngine->PushInt(3);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_ParseInt(CEngine* pEngine) {
    CValue value;
    pEngine->Pop(value);
    switch (value.mType) {
    case CValue::TYPE_INT:
        pEngine->Push(value);
        break;
    case CValue::TYPE_FLOAT:
        pEngine->PushInt(value.ToInt());
        break;
    case CValue::TYPE_STRING:
    case CValue::TYPE_STRING_PTR:
        pEngine->PushInt(pEngine->ParseInt(value));
        break;
    case CValue::TYPE_WSTRING:
    case CValue::TYPE_WSTRING_PTR:
        pEngine->PushInt(pEngine->ParseWInt(value));
        break;
    default:
        pEngine->PushInt(0);
        break;
    }
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_ParseReal(CEngine* pEngine) {
    CValue value;
    pEngine->Pop(value);
    switch (value.mType) {
    case CValue::TYPE_INT:
        pEngine->PushFloat(value.ToFloat());
        break;
    case CValue::TYPE_FLOAT:
        pEngine->Push(value);
        break;
    case CValue::TYPE_STRING:
    case CValue::TYPE_STRING_PTR:
        pEngine->PushFloat(strtof(pEngine->ToString(value), nullptr));
        break;
    case CValue::TYPE_WSTRING:
    case CValue::TYPE_WSTRING_PTR:
        pEngine->PushFloat(wcstof(pEngine->ToWString(value), nullptr));
        break;
    default:
        pEngine->PushInt(0);
        break;
    }
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_String(CEngine* pEngine) {
    int argCount = pEngine->Fetch();
    if (argCount < 1) {
        pEngine->PushString("");
        return CEngine::NATIVE_CONTINUE;
    }

    CValue value;
    pEngine->GetArg(argCount - 1, value);
    if (value.IsString()) {
        std::string result(pEngine->ToString(value));
        for (int i = argCount - 2; i >= 0; i--) {
            pEngine->GetArg(i, value);
            result = result + pEngine->ToStdString(value);
        }
        pEngine->Drop(argCount);
        pEngine->PushString(pEngine->AllocString(result));
    } else if (value.IsWString()) {
        std::wstring result(pEngine->ToWString(value));
        for (int i = argCount - 2; i >= 0; i--) {
            pEngine->GetArg(i, value);
            result = result + pEngine->ToStdWString(value);
        }
        pEngine->Drop(argCount);
        pEngine->PushWString(pEngine->AllocWString(result));
    } else {
        pEngine->Drop(argCount);
        pEngine->PushString("");
    }
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_GC(CEngine* pEngine) {
    CEngine::CollectGarbage(pEngine->PopInt(), 0);
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Dump(CEngine* pEngine) {
    int argCount = pEngine->Fetch();
    for (int i = argCount - 1; i >= 0; i--) {
        const char* pString;
        const wchar_t* pWString;
        CValue value;
        pEngine->GetArg(i, value);
        switch (value.mType) {
        case CValue::TYPE_INT:
            value.ToInt();
            break;
        case CValue::TYPE_FLOAT:
            value.ToFloat();
            break;
        case CValue::TYPE_STRING:
            pString = pEngine->ToString(value);
            break;
        case CValue::TYPE_STRING_PTR:
            pString = pEngine->ToString(value);
            break;
        case CValue::TYPE_WSTRING:
            pWString = pEngine->ToWString(value);
            break;
        case CValue::TYPE_WSTRING_PTR:
            pWString = pEngine->ToWString(value);
            break;
        }

        switch (value.mType) {
        case CValue::TYPE_STRING:
        case CValue::TYPE_STRING_PTR:
            if (pString != nullptr) {
                while (static_cast<u8>(*pString) != 0) {
                }
            }
            break;
        case CValue::TYPE_WSTRING:
        case CValue::TYPE_WSTRING_PTR:
            if (pWString != nullptr) {
                while (*pWString != L'\0') {
                }
            }
            break;
        }
    }
    pEngine->Drop(argCount);
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}