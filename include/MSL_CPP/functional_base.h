#ifndef MSL_CPP_FUNCTIONAL_BASE_H
#define MSL_CPP_FUNCTIONAL_BASE_H

namespace std {
template <class First, class Second, class Result> struct binary_function {
    typedef First first_argument_type;
    typedef Second second_argument_type;
    typedef Result result_type;
};

template <typename T> struct less : binary_function<T, T, bool> {
    bool operator()(const T& a, const T& b) const { return a < b; }
};
}

#endif
