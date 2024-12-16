#include "cortex/coroutine.hpp"

#include <exception>

#include <boost/context/fiber.hpp>

namespace cortex {

namespace aux {
struct fiber_suspend_context : public coroutine::suspend_context {
    explicit fiber_suspend_context(boost::context::fiber& sink) noexcept
        : _sink(sink) {}
    fiber_suspend_context(const fiber_suspend_context&) = delete;
    fiber_suspend_context(fiber_suspend_context&&) noexcept = delete;
    fiber_suspend_context& operator=(const fiber_suspend_context&) = delete;
    fiber_suspend_context& operator=(fiber_suspend_context&&) noexcept = delete;

    ~fiber_suspend_context() override = default;

    void suspend() override {
        _sink = std::move(_sink).resume();
    }

private:
    boost::context::fiber& _sink;
};
} // namespace aux

struct coroutine::coroutine_impl {
    bool is_done {};
    std::exception_ptr exception {};
    boost::context::fiber fiber {};
};

coroutine::coroutine(std::unique_ptr<coroutine_impl> impl) noexcept
    : _impl(std::move(impl)) {}

coroutine coroutine::create(flow_t in_flow) {
    auto impl = std::make_unique<coroutine_impl>();

    boost::context::fiber source(
        [impl_raw_ptr = impl.get(), flow = std::move(in_flow)](boost::context::fiber&& sink) mutable {
            assert(impl_raw_ptr != nullptr);
            impl_raw_ptr->is_done = false;

            aux::fiber_suspend_context suspender(sink);
            suspender.suspend();

            try {
                flow(suspender);
            } catch (const std::exception&) {
                assert(impl_raw_ptr != nullptr);
                impl_raw_ptr->exception = std::current_exception();
            }

            assert(impl_raw_ptr != nullptr);
            impl_raw_ptr->is_done = true;
            return std::move(sink);
        });

    impl->fiber = std::move(source).resume();

    return coroutine(std::move(impl));
}

coroutine::~coroutine() = default;

void coroutine::resume() {
    assert(_impl);
    if (is_done()) {
        throw resume_on_completed_coroutine_error_t {"resume on finished coroutine"};
    }

    _impl->fiber = std::move(_impl->fiber).resume();

    if (_impl->exception) {
        std::rethrow_exception(_impl->exception);
    }
}

bool coroutine::is_done() const noexcept {
    assert(_impl);
    return _impl->is_done;
}

} // namespace cortex
