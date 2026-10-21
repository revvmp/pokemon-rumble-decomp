#ifndef MSL_CPP_RUNTIME_ERROR_H
#define MSL_CPP_RUNTIME_ERROR_H

#include <exception>
#include <memory>
#include <string.h>

namespace std {

class __error_string {
public:
    explicit __error_string(const char* pText) : text_(new char[strlen(pText) + 1], detail::default_delete<char[]>()) {
        strcpy(text_.get(), pText);
    }
    const char* c_str() const { return text_.get(); }

private:
    tr1::shared_ptr<char> text_;
};

class runtime_error : public exception {
public:
    explicit runtime_error(const char* pWhat) : what_(pWhat) {}
    virtual ~runtime_error() {}
    virtual const char* what() const { return what_.c_str(); }

private:
    __error_string what_;
};

}

#endif
