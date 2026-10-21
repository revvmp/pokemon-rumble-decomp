#include <revolution/types.h>
#include <NIntl/basic_formatter.h>
#include <game/StringFormat.h>


namespace NIntl {

template <> basic_formatter<char>& basic_formatter<char>::operator%(const int& rArg) {
    mArgs.push_back(FormatString(mSpec.size() != 0 ? mSpec.c_str() : "%d", rArg));
    mParsed = false;
    mSpec.clear();
    return *this;
}

template <> basic_formatter<char>& basic_formatter<char>::operator%(const float& rArg) {
    mArgs.push_back(FormatString(mSpec.size() != 0 ? mSpec.c_str() : "%f", rArg));
    mParsed = false;
    mSpec.clear();
    return *this;
}

template <> basic_formatter<char>& basic_formatter<char>::operator%(const char* pArg) {
    mArgs.push_back(FormatString(mSpec.size() != 0 ? mSpec.c_str() : "%s", pArg));
    mParsed = false;
    mSpec.clear();
    return *this;
}

template <> basic_formatter<char>& basic_formatter<char>::operator%(const wchar_t* pArg) {
    mArgs.push_back(FormatString(mSpec.size() != 0 ? mSpec.c_str() : "%ls", pArg));
    mParsed = false;
    mSpec.clear();
    return *this;
}

template <> basic_formatter<wchar_t>& basic_formatter<wchar_t>::operator%(const int& rArg) {
    mArgs.push_back(FormatString(mSpec.size() != 0 ? mSpec.c_str() : L"%d", rArg));
    mParsed = false;
    mSpec.clear();
    return *this;
}

template <> basic_formatter<wchar_t>& basic_formatter<wchar_t>::operator%(const unsigned long& rArg) {
    mArgs.push_back(FormatString(mSpec.size() != 0 ? mSpec.c_str() : L"%lu", rArg));
    mParsed = false;
    mSpec.clear();
    return *this;
}

template <> basic_formatter<wchar_t>& basic_formatter<wchar_t>::operator%(const float& rArg) {
    mArgs.push_back(FormatString(mSpec.size() != 0 ? mSpec.c_str() : L"%f", rArg));
    mParsed = false;
    mSpec.clear();
    return *this;
}

template <> basic_formatter<wchar_t>& basic_formatter<wchar_t>::operator%(const char* pArg) {
    mArgs.push_back(FormatString(mSpec.size() != 0 ? mSpec.c_str() : L"%s", pArg));
    mParsed = false;
    mSpec.clear();
    return *this;
}

template <> basic_formatter<wchar_t>& basic_formatter<wchar_t>::operator%(const wchar_t* pArg) {
    mArgs.push_back(FormatString(mSpec.size() != 0 ? mSpec.c_str() : L"%ls", pArg));
    mParsed = false;
    mSpec.clear();
    return *this;
}

}

