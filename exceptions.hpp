#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

/*
 * You don't have to implement exceptions.hpp.
 * Just remember to throw exception when needed.
 */
namespace sjtu {

class exception {
protected:
    const std::string variant = "";
    std::string detail = "";
public:
    exception() {}
    exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
    virtual std::string what() {
        return variant + " " + detail;
    }
};

class index_out_of_bound : public exception {
public:
    index_out_of_bound(const std::string &msg = "") {
        detail = msg;
        const_cast<std::string&>(variant) = "Index Out of Bound";
    }
};

class runtime_error : public exception {
public:
    runtime_error(const std::string &msg = "") {
        detail = msg;
        const_cast<std::string&>(variant) = "Runtime Error";
    }
};

class invalid_iterator : public exception {
public:
    invalid_iterator(const std::string &msg = "") {
        detail = msg;
        const_cast<std::string&>(variant) = "Invalid Iterator";
    }
};

class container_is_empty : public exception {
public:
    container_is_empty(const std::string &msg = "") {
        detail = msg;
        const_cast<std::string&>(variant) = "Container is Empty";
    }
};
}

#endif
