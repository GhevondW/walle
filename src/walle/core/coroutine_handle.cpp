#include "walle/core/coroutine_handle.hpp"
#include "walle/core/error.hpp"

namespace walle::core {

struct coroutine_handle::impl {};

coroutine_handle::coroutine_handle(std::unique_ptr<typename coroutine_handle::impl> impl) noexcept
    : _impl(std::move(impl)) {}

coroutine_handle coroutine_handle::create([[maybe_unused]] flow_t flow) {
    throw not_implemeted_error {};
}

coroutine_handle::~coroutine_handle() noexcept = default;

coroutine_handle::coroutine_handle(coroutine_handle&& other) noexcept
    : _impl(std::move(other._impl)) {}

coroutine_handle& coroutine_handle::operator=(coroutine_handle&& other) noexcept {
    if (this != &other) {
        // TODO : destroy impl and replace
    }
    return *this;
}

void coroutine_handle::resume() {}

[[nodiscard]] bool coroutine_handle::is_done() const noexcept {
    // impl
    return false;
}

} // namespace walle::core
