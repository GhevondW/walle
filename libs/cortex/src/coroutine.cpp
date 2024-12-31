#include "cortex/coroutine.hpp"

#include <boost/context/fiber.hpp>
#include <cassert>
#include <exception>
#include <memory>

namespace ctx = boost::context;

namespace walle::cortex {

struct fiber_suspend_context : coroutine::suspend_context {
    fiber_suspend_context(ctx::fiber& sink)
        : _sink(sink) {}

    ~fiber_suspend_context() override = default;

    void suspend() override {
        _sink = std::move(_sink).resume();
    }

private:
    ctx::fiber& _sink;
};

struct coroutine::impl {
    ctx::fiber fiber {};
    std::exception_ptr error {};
    bool is_done {false};
};

coroutine::coroutine(std::shared_ptr<impl> impl) noexcept
    : _impl(std::move(impl)) {}

coroutine coroutine::create([[maybe_unused]] flow_t in_flow) {
    auto impl = std::make_shared<coroutine::impl>();

    ctx::fiber fiber(
        [weak_impl = std::weak_ptr<coroutine::impl>(impl), flow = std::move(in_flow)](ctx::fiber&& sink) mutable {
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

    impl->fiber = std::move(fiber).resume();
    return coroutine(std::move(impl));
}

coroutine::~coroutine() = default;

coroutine::coroutine(coroutine&& other) noexcept
    : _impl(std::move(other._impl)) {}

// if (is_done()) {
//     throw resume_on_completed_coroutine_error_t {"resume on finished coroutine"};
// }

// _impl->_machine_context = boost::context::detail::jump_fcontext(_impl->_machine_context, _impl.get()).fctx;

// if (_impl->_exception) {
//     std::rethrow_exception(_impl->_exception);
// }

void coroutine::resume() {
    assert(_impl);
    if (is_done()) {
        throw resume_on_completed_coroutine_error_t {"resume on finished coroutine"};
    }

    _impl->fiber = std::move(_impl->fiber).resume();

    if (_impl->error) {
        std::rethrow_exception(_impl->error);
    }
}

[[nodiscard]] bool coroutine::is_done() const noexcept {
    assert(_impl);
    return _impl->is_done;
}

} // namespace walle::cortex
