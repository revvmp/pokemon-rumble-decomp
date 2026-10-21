#ifndef GAME_STRINGFORMAT_H
#define GAME_STRINGFORMAT_H

#include <stdarg.h>
#include <string>

int VFormatString(std::string& rResult, const char* pFormat, va_list args);
int VFormatString(std::wstring& rResult, const wchar_t* pFormat, va_list args);
std::string FormatString(const char* pFormat, ...);
std::wstring FormatString(const wchar_t* pFormat, ...);

#endif
