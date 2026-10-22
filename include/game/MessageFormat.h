#ifndef GAME_MESSAGEFORMAT_H
#define GAME_MESSAGEFORMAT_H

#include <NIntl/basic_formatter.h>
#include <string>

template <class A> std::wstring FormatMessage(const wchar_t* pFormat, const A& rA) {
    NIntl::basic_formatter<wchar_t> formatter(pFormat);
    return (formatter % rA).str();
}

template <class A, class B> std::wstring FormatMessage(const wchar_t* pFormat, const A& rA, const B& rB) {
    NIntl::basic_formatter<wchar_t> formatter(pFormat);
    return (formatter % rA % rB).str();
}

#endif
