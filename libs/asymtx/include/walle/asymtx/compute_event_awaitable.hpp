#pragma once

#include <boost/compute/event.hpp>
#include <coroutine>

namespace walle::asymtx {

class compute_event_awaitable_t {
public:
    explicit compute_event_awaitable_t(boost::compute::event&& event) noexcept
        : _event(std::move(event)) {}

    bool await_ready() noexcept;

    void await_suspend(std::coroutine_handle<> awaiting_coroutine);

    void await_resume() noexcept;

private:
    boost::compute::event _event;
};

} // namespace walle::asymtx
