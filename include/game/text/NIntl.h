#ifndef GAME_NINTL_H
#define GAME_NINTL_H

#include <string>
#include <vector>
#include <MSL_C/ctype.h>

namespace NIntl {

template <typename CharT> class basic_formatter {
public:
    typedef std::basic_string<CharT> string_type;

    class InvalidFormatException {};
    class TooFewArgsException {};
    class TooManyArgsException {};

    explicit basic_formatter(const CharT* pFormat) : mFormat(pFormat), mFormatted(false) {}

    struct format_spec {
        explicit format_spec(const CharT* pSpec) : mpSpec(pSpec) {}
        const CharT* mpSpec;
    };

    basic_formatter& operator%(const format_spec& rSpec) {
        mScratch = rSpec.mpSpec;
        return *this;
    }
    basic_formatter& operator%(const CharT* pArg);
    basic_formatter& operator%(const int& rArg);

    const string_type& str() {
        if (!mFormatted) {
            mResult.clear();
            mFormatted = false;
            unsigned long maxIndex = 0;
            for (unsigned long i = 0; i < mFormat.size(); i++) {
                int c = mFormat[i];
                if (c == '{') {
                    if (i + 2 >= mFormat.size() || !isdigit(mFormat[i + 1]) || mFormat[i + 2] != '}') {
                        throw InvalidFormatException();
                    } else {
                        unsigned long index = mFormat[i + 1] - '0';
                        if (index < 1) {
                            throw InvalidFormatException();
                        }
                        if (maxIndex < index) {
                            maxIndex = index;
                        }
                        if (index >= 1 && index <= mArgs.size()) {
                            mResult += mArgs[index - 1];
                        } else {
                            throw TooFewArgsException();
                        }
                        i += 2;
                    }
                } else {
                    CharT ch = c;
                    mResult += ch;
                }
            }
            mFormatted = true;
            if (maxIndex < mArgs.size()) {
                throw TooManyArgsException();
            }
        }
        return mResult;
    }

private:
    string_type mFormat;
    std::vector<string_type> mArgs;
    string_type mScratch;
    string_type mResult;
    bool mFormatted;
};

}

#endif
