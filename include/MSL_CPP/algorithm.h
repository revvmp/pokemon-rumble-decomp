#ifndef MSL_CPP_ALGORITHM_H
#define MSL_CPP_ALGORITHM_H
namespace std {
template <typename TPtr, typename T>
inline TPtr find(TPtr first, TPtr last, const T& value) {
    while (first != last && *first != value) {
        ++first;
    }

    return first;
}

template <typename T> inline T& move(T& x) {
    return x;
}

template <typename T> inline void swap(T& a, T& b) {
    T tmp = move(a);
    a = move(b);
    b = move(tmp);
}

}
#endif
