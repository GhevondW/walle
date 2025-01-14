#include "walle/cortex/coroutine_base.hpp"

#include <utility>

namespace walle::cortex {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
coroutine_base_i::coroutine_base_i()
    : _coroutine(coroutine_t::create([this](auto& self) { run_flow(self); })) {}
#pragma GCC diagnostic pop

coroutine_base_i::coroutine_base_i(coroutine_base_i&& other) noexcept
    : _coroutine(std::move(other._coroutine)) {}

coroutine_base_i& coroutine_base_i::operator=(coroutine_base_i&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    auto tmp = coroutine_t::create();
    std::swap(_coroutine, tmp);
    return *this;
}

coroutine_base_i::operator bool() const noexcept {
    return static_cast<bool>(_coroutine);
}

bool coroutine_base_i::is_valid() const noexcept {
    return _coroutine.is_valid();
}

void coroutine_base_i::resume() {
    return _coroutine.resume();
}

bool coroutine_base_i::is_done() const noexcept {
    return _coroutine.is_done();
}

void coroutine_base_i::run_flow(suspend_context_i& self) {
    flow(self);
}

} // namespace walle::cortex
