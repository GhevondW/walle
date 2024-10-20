#pragma once

#include <string>

namespace walle::core {

class error : public std::exception {
public:
    template <typename StringLike>
    explicit error(StringLike&& str)
        : _what(std::forward<StringLike>(str)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return _what.c_str();
    }

private:
    std::string _what;
};

class not_implemeted_error : public error {
public:
    using error::error;
};

class logic_error : public error {
public:
    using error::error;
};

} // namespace walle::core
