#ifndef MSL_CPP_COMPRESSED_PAIR_H
#define MSL_CPP_COMPRESSED_PAIR_H

namespace Metrowerks {
namespace detail {
template <class T> struct IsClass {
    template <class U> static char Probe(int U::*);
    template <class U> static long Probe(...);
    enum { value = sizeof(Probe<T>(0)) == sizeof(char) };
};
template <class T, bool IsClassType> struct IsEmptyClass { enum { value = false }; };
template <class T> struct IsEmptyClass<T, true> {
    struct Derived : T { char mValue; };
    enum { value = sizeof(Derived) == sizeof(char) };
};
template <class First, class Second> struct IsSame { enum { value = false }; };
template <class T> struct IsSame<T, T> { enum { value = true }; };
template <class First, class Second, int Storage> class CompressedPairStorage;
template <class First, class Second> class CompressedPairStorage<First, Second, 0> {
    First mFirst;
    Second mSecond;
public:
    CompressedPairStorage() : mFirst(), mSecond() {}
    CompressedPairStorage(const First& rFirst, const Second& rSecond) : mFirst(rFirst), mSecond(rSecond) {}
    First& first() { return mFirst; }
    const First& first() const { return mFirst; }
    Second& second() { return mSecond; }
    const Second& second() const { return mSecond; }
};
template <class First, class Second> class CompressedPairStorage<First, Second, 1> : private First {
    Second mSecond;
public:
    CompressedPairStorage() : mSecond() {}
    CompressedPairStorage(const First& rFirst, const Second& rSecond) : First(rFirst), mSecond(rSecond) {}
    First& first() { return *this; }
    const First& first() const { return *this; }
    Second& second() { return mSecond; }
    const Second& second() const { return mSecond; }
};
template <class First, class Second> class CompressedPairStorage<First, Second, 2> : private Second {
    First mFirst;
public:
    CompressedPairStorage() : mFirst() {}
    CompressedPairStorage(const First& rFirst, const Second& rSecond) : Second(rSecond), mFirst(rFirst) {}
    First& first() { return mFirst; }
    const First& first() const { return mFirst; }
    Second& second() { return *this; }
    const Second& second() const { return *this; }
};
template <class First, class Second> class CompressedPairStorage<First, Second, 3> : private First, private Second {
public:
    CompressedPairStorage() {}
    CompressedPairStorage(const First& rFirst, const Second& rSecond) : First(rFirst), Second(rSecond) {}
    First& first() { return *this; }
    const First& first() const { return *this; }
    Second& second() { return *this; }
    const Second& second() const { return *this; }
};
template <class First, class Second> struct PairStorageKind {
    enum {
        firstEmpty = IsEmptyClass<First, IsClass<First>::value>::value,
        secondEmpty = IsEmptyClass<Second, IsClass<Second>::value>::value,
        value = firstEmpty ? (secondEmpty && !IsSame<First, Second>::value ? 3 : 1) : (secondEmpty ? 2 : 0)
    };
};
}
template <class First, class Second>
class compressed_pair : private detail::CompressedPairStorage<First, Second, detail::PairStorageKind<First, Second>::value> {
    typedef detail::CompressedPairStorage<First, Second, detail::PairStorageKind<First, Second>::value> Storage;
public:
    compressed_pair() {}
    compressed_pair(const First& rFirst, const Second& rSecond) : Storage(rFirst, rSecond) {}
    First& first() { return Storage::first(); }
    const First& first() const { return Storage::first(); }
    Second& second() { return Storage::second(); }
    const Second& second() const { return Storage::second(); }
};
}

#endif
