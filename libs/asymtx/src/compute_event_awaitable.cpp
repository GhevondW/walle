#include "walle/asymtx/compute_event_awaitable.hpp"

#include "walle/exec/current_executor.hpp"

namespace walle::asymtx {

bool compute_event_awaitable_t::await_ready() noexcept {
    return false;
}

void compute_event_awaitable_t::await_suspend(std::coroutine_handle<> coroutine) {
    auto executor = exec::current_executor::get();
    _event.set_callback([executor, coroutine]() { executor->submit([coroutine]() { coroutine.resume(); }); });
}

void compute_event_awaitable_t::await_resume() noexcept {}

} // namespace walle::asymtx
