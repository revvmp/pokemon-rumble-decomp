#ifndef NW4R_SND_BANK_H
#define NW4R_SND_BANK_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BankFile.h>
#include <nw4r/snd/snd_NoteOnCallback.h>

namespace nw4r {
namespace snd {
namespace detail {

class Channel;

class Bank {
public:
    explicit Bank(const void* pBankBin);
    ~Bank();

    Channel* NoteOn(const NoteOnInfo& rInfo) const;

    void SetWaveDataAddress(const void* pData) {
        mWaveDataAddress = pData;
    }

private:
    BankFileReader mBankReader;
    const void* mWaveDataAddress;
};

}
}
}

#endif
