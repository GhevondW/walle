#include "walle/cortex/coroutine.hpp"
#include "walle/cortex/error.hpp"

#include <boost/context/fiber.hpp>
#include <cassert>
#include <exception>
#include <memory>
#include <utility>

namespace ctx = boost::context;

namespace walle::cortex {

struct fiber_suspend_context : suspend_context_i {
    fiber_suspend_context(ctx::fiber& sink)
        : _sink(sink) {}

    ~fiber_suspend_context() override = default;

    void suspend() override {
        _sink = std::move(_sink).resume();
    }

private:
    ctx::fiber& _sink;
};

struct coroutine_t::impl {
    ctx::fiber fiber {};
    std::exception_ptr error {};
    bool is_done {false};
};

coroutine_t::coroutine_t(std::shared_ptr<impl> in_impl) noexcept
    : _impl(std::move(in_impl)) {}

coroutine_t coroutine_t::create() {
    return coroutine_t(nullptr);
}

coroutine_t coroutine_t::create(flow_t in_flow) {
    auto coro_impl = std::make_shared<coroutine_t::impl>();

    ctx::fiber fiber([weak_impl = std::weak_ptr<coroutine_t::impl>(coro_impl),
                      flow = std::move(in_flow)](ctx::fiber&& sink) mutable {
        assert(!weak_impl.expired());

        fiber_suspend_context suspend_context(sink);
        suspend_context.suspend();

        try {
            flow(suspend_context);
        } catch (const std::exception& error) {
            if (auto impl = weak_impl.lock(); impl) {
                impl->error = std::current_exception();
            }
        }

        if (auto impl = weak_impl.lock(); impl) {
            impl->is_done = true;
        }
        return std::move(sink).resume();
    });

    coro_impl->fiber = std::move(fiber).resume();
    return coroutine_t(std::move(coro_impl));
}

coroutine_t::~coroutine_t() = default;

coroutine_t::coroutine_t(coroutine_t&& other) noexcept
    : _impl(std::move(other._impl)) {}

coroutine_t& coroutine_t::operator=(coroutine_t&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    auto tmp = std::make_shared<coroutine_t::impl>();
    std::swap(tmp, _impl);
    return *this;
}

coroutine_t::operator bool() const noexcept {
    return is_valid();
}

bool coroutine_t::is_valid() const noexcept {
    return static_cast<bool>(_impl);
}

void coroutine_t::resume() {
    assert(_impl);
    if (is_done()) {
        throw resume_on_completed_coroutine_error_t {"resume on finished coroutine"};
    }

    _impl->fiber = std::move(_impl->fiber).resume();

    if (_impl->error) {
        std::rethrow_exception(_impl->error);
    }
}

[[nodiscard]] bool coroutine_t::is_done() const noexcept {
    assert(_impl);
    return _impl->is_done;
}

} // namespace walle::cortex
