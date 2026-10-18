#ifndef MSL_CPP_MSL_UTILITY_H
#define MSL_CPP_MSL_UTILITY_H

namespace Metrowerks {

template <int N> struct int2type {
    enum { value = N };
};

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

}

#endif
