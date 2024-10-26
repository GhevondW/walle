#include "walle/core/coroutine/handle.hpp"
#include "walle/core/error.hpp"

// TODO : do not include a header from detail
#include <boost/context/detail/fcontext.hpp>

namespace walle::core::coroutine {

struct handle::impl {
    boost::context::detail::fcontext_t _machine_context;
};

handle::handle(std::unique_ptr<typename handle::impl> impl) noexcept
    : _impl(std::move(impl)) {}

handle handle::create([[maybe_unused]] flow_t flow) {
    throw not_implemeted_error {};
}

handle::~handle() noexcept = default;

handle::handle(handle&& other) noexcept
    : _impl(std::move(other._impl)) {}

handle& handle::operator=(handle&& other) noexcept {
    if (this != &other) {
        // TODO : destroy impl and replace
    }
    return *this;
}

void handle::resume() {}

[[nodiscard]] bool handle::is_done() const noexcept {
    // impl
    return false;
}

} // namespace walle::core::coroutine
