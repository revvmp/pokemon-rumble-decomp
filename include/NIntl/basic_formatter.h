#ifndef NINTL_BASIC_FORMATTER_H
#define NINTL_BASIC_FORMATTER_H

#include <string>
#include <vector>
#include <MSL_C/ctype.h>

namespace NIntl {

template <class charT> class basic_formatter {
public:
    class InvalidFormatException {};
    class TooFewArgsException {};
    class TooManyArgsException {};

    explicit basic_formatter(const charT* pFormat);
    ~basic_formatter() {}

    basic_formatter& operator%(const int& rArg);
    basic_formatter& operator%(const unsigned long& rArg);
    basic_formatter& operator%(const float& rArg);
    basic_formatter& operator%(const char* pArg);
    basic_formatter& operator%(const wchar_t* pArg);

    basic_formatter& format(const charT* pSpec) {
        mSpec = pSpec;
        return *this;
    }

    const std::basic_string<charT>& str() {
        if (!mParsed) {
            parse();
        }
        return mResult;
    }

    const charT* c_str() { return str().c_str(); }

private:
    void parse() {
        mResult.clear();
        mParsed = false;
        unsigned long maxArg = 0;
        for (unsigned long i = 0; i < mFormat.size(); i++) {
            int c = mFormat[i];
            if (c == '{') {
                if (!(i + 2 < mFormat.size() && isdigit(mFormat[i + 1]) && mFormat[i + 2] == '}')) {
                    throw InvalidFormatException();
                } else {
                    unsigned long index = mFormat[i + 1] - '0';
                    if (index < 1) {
                        throw InvalidFormatException();
                    }
                    if (maxArg < index) {
                        maxArg = index;
                    }
                    if (index >= 1 && index <= mArgs.size()) {
                        mResult += mArgs[index - 1];
                    } else {
                        throw TooFewArgsException();
                    }
                    i += 2;
                }
            } else {
                mResult += static_cast<charT>(c);
            }
        }
        mParsed = true;
        if (maxArg < mArgs.size()) {
            throw TooManyArgsException();
        }
    }

    std::basic_string<charT> mFormat;
    std::vector<std::basic_string<charT> > mArgs;
    std::basic_string<charT> mSpec;
    std::basic_string<charT> mResult;
    bool mParsed;
};

template <class charT> inline basic_formatter<charT>::basic_formatter(const charT* pFormat) : mFormat(pFormat), mParsed(false) {}

}

#endif
