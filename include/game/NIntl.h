#ifndef GAME_NINTL_H
#define GAME_NINTL_H

#include <string>
#include <vector>
#include <wctype.h>

namespace NIntl {

template <typename CharT> class basic_formatter {
public:
    typedef std::basic_string<CharT> string_type;

    class InvalidFormatException {};
    class TooFewArgsException {};
    class TooManyArgsException {};

    explicit basic_formatter(const CharT* pFormat) : mFormat(pFormat), mFormatted(false) {}

    basic_formatter& operator%(const CharT* pArg);
    basic_formatter& operator%(int arg);

    const string_type& str() {
        if (!mFormatted) {
            mResult.clear();
            mFormatted = false;
            unsigned long maxIndex = 0;
            for (unsigned long i = 0; i < mFormat.size(); i++) {
                if (mFormat[i] == '{') {
                    if (i + 2 >= mFormat.size() || !iswdigit(mFormat[i + 1]) || mFormat[i + 2] != '}') {
                        throw InvalidFormatException();
                    }
                    unsigned long index = mFormat[i + 1] - '0';
                    if (index < 1) {
                        throw InvalidFormatException();
                    }
                    if (maxIndex < index) {
                        maxIndex = index;
                    }
                    if (index >= 1 && index <= mArgs.size()) {
                        const string_type& rArg = mArgs[index - 1];
                        mResult.append(rArg.begin(), rArg.end());
                    } else {
                        throw TooFewArgsException();
                    }
                    i += 2;
                } else {
                    mResult.append(&mFormat[i], &mFormat[i] + 1);
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
