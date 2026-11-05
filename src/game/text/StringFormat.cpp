#include <revolution/types.h>
#include <game/text/StringFormat.h>
#include <stdio.h>
#include <wchar.h>

extern "C" int __CountPrintf(const char* pFormat, va_list args);
extern "C" int __wCountPrintf(const wchar_t* pFormat, va_list args);

int VFormatString(std::string& rResult, const char* pFormat, va_list args) {
    va_list copy;
    va_copy(copy, args);
    int count = __CountPrintf(pFormat, copy);
    if (count >= 0) {
        rResult.resize(count);
        va_copy(copy, args);
        vsnprintf(const_cast<char*>(rResult.c_str()), rResult.size() + 1, pFormat, copy);
    }
    return count;
}

int VFormatString(std::wstring& rResult, const wchar_t* pFormat, va_list args) {
    va_list copy;
    va_copy(copy, args);
    int count = __wCountPrintf(pFormat, copy);
    if (count >= 0) {
        rResult.resize(count);
        va_copy(copy, args);
        vswprintf(const_cast<wchar_t*>(rResult.c_str()), rResult.size() + 1, pFormat, copy);
    }
    return count;
}

