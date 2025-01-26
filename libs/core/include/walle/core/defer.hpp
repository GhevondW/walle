#pragma once

#include <utility>

namespace walle::core {

template <typename Func>
class defer_t {
public:
    explicit defer_t(Func&& func)
        : _func(std::move(func)) {}

    ~defer_t() noexcept(noexcept(std::declval<Func&>()())) {
        _func();
    }

private:
    Func _func;
};

} // namespace walle::core
