#ifndef MSL_CPP_MSL_UTILITY_H
#define MSL_CPP_MSL_UTILITY_H

namespace Metrowerks {

template <int N> struct int2type {
    enum { value = N };
};

template <typename T, typename U> struct is_same { enum { value = false }; };
template <typename T> struct is_same<T, T> { enum { value = true }; };
template <typename T> struct has_trivial_dtor { enum { value = (__builtin_trivial_members(T) & 1) != 0 }; };
template <typename T> struct has_trivial_dtor<T&> { enum { value = true }; };

template <typename T> struct is_POD {
    enum { value = false };
};

template <typename T> struct is_POD<T*> {
    enum { value = true };
};

template <> struct is_POD<bool> {
    enum { value = true };
};

template <> struct is_POD<char> {
    enum { value = true };
};

template <> struct is_POD<signed char> {
    enum { value = true };
};

template <> struct is_POD<unsigned char> {
    enum { value = true };
};

template <> struct is_POD<short> {
    enum { value = true };
};

template <> struct is_POD<unsigned short> {
    enum { value = true };
};

template <> struct is_POD<int> {
    enum { value = true };
};

template <> struct is_POD<unsigned int> {
    enum { value = true };
};

template <> struct is_POD<long> {
    enum { value = true };
};

template <> struct is_POD<unsigned long> {
    enum { value = true };
};

template <> struct is_POD<long long> {
    enum { value = true };
};

template <> struct is_POD<unsigned long long> {
    enum { value = true };
};

template <> struct is_POD<float> {
    enum { value = true };
};

template <> struct is_POD<double> {
    enum { value = true };
};

template <class T> struct remove_const { typedef T type; };
template <class T> struct remove_const<const T> { typedef T type; };
template <class T> struct remove_volatile { typedef T type; };
template <class T> struct remove_volatile<volatile T> { typedef T type; };
template <class T> struct remove_cv { typedef typename remove_const<typename remove_volatile<T>::type>::type type; };
namespace details {
template <class T> struct is_integral_imp { enum { value = false }; };
template <> struct is_integral_imp<bool> { enum { value = true }; };
template <> struct is_integral_imp<char> { enum { value = true }; };
template <> struct is_integral_imp<signed char> { enum { value = true }; };
template <> struct is_integral_imp<unsigned char> { enum { value = true }; };
template <> struct is_integral_imp<short> { enum { value = true }; };
template <> struct is_integral_imp<unsigned short> { enum { value = true }; };
template <> struct is_integral_imp<int> { enum { value = true }; };
template <> struct is_integral_imp<unsigned int> { enum { value = true }; };
template <> struct is_integral_imp<long> { enum { value = true }; };
template <> struct is_integral_imp<unsigned long> { enum { value = true }; };
template <> struct is_integral_imp<long long> { enum { value = true }; };
template <> struct is_integral_imp<unsigned long long> { enum { value = true }; };
#if defined(__MWERKS__)
#if __option(wchar_type)
template <> struct is_integral_imp<wchar_t> { enum { value = true }; };
#endif
#else
template <> struct is_integral_imp<wchar_t> { enum { value = true }; };
#endif
template <class T> struct is_floating_imp { enum { value = false }; };
template <> struct is_floating_imp<float> { enum { value = true }; };
template <> struct is_floating_imp<double> { enum { value = true }; };
template <> struct is_floating_imp<long double> { enum { value = true }; };
template <class T> struct is_pointer_imp { enum { value = false }; };
template <class T> struct is_pointer_imp<T*> { enum { value = true }; };
template <class T> struct is_member_pointer_imp { enum { value = false }; };
template <class T, class U> struct is_member_pointer_imp<U T::*> { enum { value = true }; };
#if defined(__MWERKS__) && __MWERKS__ >= 0x2500
template <class T> struct is_enum_imp { enum { value = (__builtin_type(T) & 0xff00) == 0x0400 }; };
#else
template <class T> struct is_enum_imp { enum { value = false }; };
#endif
}
template <class T> struct is_integral { enum { value = details::is_integral_imp<typename remove_cv<T>::type>::value }; };
template <class T> struct is_floating { enum { value = details::is_floating_imp<typename remove_cv<T>::type>::value }; };
template <class T> struct is_pointer { enum { value = details::is_pointer_imp<typename remove_cv<T>::type>::value }; };
template <class T> struct is_member_pointer { enum { value = details::is_member_pointer_imp<typename remove_cv<T>::type>::value }; };
template <class T> struct is_enum { enum { value = details::is_enum_imp<typename remove_cv<T>::type>::value }; };
template <class T> struct is_arithmetic { enum { value = is_integral<T>::value || is_floating<T>::value }; };
template <class T> struct is_scalar { enum { value = is_arithmetic<T>::value || is_pointer<T>::value || is_member_pointer<T>::value || is_enum<T>::value }; };

}

#endif
