#ifndef MSL_CPP_COMPRESSED_PAIR_H
#define MSL_CPP_COMPRESSED_PAIR_H

namespace Metrowerks {
namespace detail {
template <class T, bool Small> struct CallTraitsSize {
    typedef const T param_type;
};

template <class T> struct CallTraitsSize<T, false> { typedef const T& param_type; };

template <class T> struct CallTraitsStorage : CallTraitsSize<T, sizeof(T) <= sizeof(void*)> {};

template <class T> struct CallTraitsStorage<T*> { typedef T* param_type; };

template <class T> struct CallTraitsStorage<T&> { typedef T& param_type; };
}

template <class T> struct call_traits {
    typedef typename detail::CallTraitsStorage<T>::param_type param_type;
};

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
    CompressedPairStorage(typename call_traits<First>::param_type rFirst, typename call_traits<Second>::param_type rSecond) : mFirst(rFirst), mSecond(rSecond) {}
    First& first() { return mFirst; }
    const First& first() const { return mFirst; }
    Second& second() { return mSecond; }
    const Second& second() const { return mSecond; }
};
template <class First, class Second> class CompressedPairStorage<First, Second, 1> : private First {
    Second mSecond;
public:
    CompressedPairStorage() : mSecond() {}
    explicit CompressedPairStorage(typename call_traits<First>::param_type rFirst) : First(rFirst), mSecond() {}
    CompressedPairStorage(typename call_traits<First>::param_type rFirst, typename call_traits<Second>::param_type rSecond) : First(rFirst), mSecond(rSecond) {}
    First& first() { return *this; }
    const First& first() const { return *this; }
    Second& second() { return mSecond; }
    const Second& second() const { return mSecond; }
};
template <class First, class Second> class CompressedPairStorage<First, Second, 2> : private Second {
    First mFirst;
public:
    CompressedPairStorage() : mFirst() {}
    explicit CompressedPairStorage(typename call_traits<First>::param_type rFirst) : mFirst(rFirst) {}
    CompressedPairStorage(typename call_traits<First>::param_type rFirst, typename call_traits<Second>::param_type rSecond) : Second(rSecond), mFirst(rFirst) {}
    First& first() { return mFirst; }
    const First& first() const { return mFirst; }
    Second& second() { return *this; }
    const Second& second() const { return *this; }
};
template <class First, class Second> class CompressedPairStorage<First, Second, 3> : private First, private Second {
public:
    CompressedPairStorage() {}
    CompressedPairStorage(typename call_traits<First>::param_type rFirst, typename call_traits<Second>::param_type rSecond) : First(rFirst), Second(rSecond) {}
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
    typedef typename call_traits<First>::param_type first_param;
    typedef typename call_traits<Second>::param_type second_param;
    compressed_pair() {}
    explicit compressed_pair(first_param rFirst) : Storage(rFirst) {}
    compressed_pair(first_param rFirst, second_param rSecond) : Storage(rFirst, rSecond) {}
    First& first() { return Storage::first(); }
    const First& first() const { return Storage::first(); }
    Second& second() { return Storage::second(); }
    const Second& second() const { return Storage::second(); }
};
}

#endif
