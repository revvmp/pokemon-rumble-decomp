#include <game/NPpuVM.h>

#include <stdio.h>
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
    wchar_t* pIn = format;
    int arg = 1 - argCount;
    wchar_t* pOut = buffer;
    wchar_t* pLimit = buffer + 1024;
    wcsncpy(format, pFormat, 1024);
    format[1023] = L'\0';

    while (pOut < pLimit - 1 && *pIn != L'\0' && arg < 0) {
        if (*pIn != L'%') {
            *pOut++ = *pIn++;
            continue;
        }

        wchar_t* pSpec = pIn;
        wchar_t* pEnd = pIn + 1;
        int size = 0;
        wchar_t type;
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
    char* pIn = format;
    int arg = 1 - argCount;
    char* pOut = buffer;
    char* pLimit = buffer + 1024;
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
        char type;
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
