#include "fiber.hpp"
#include <cassert>
#include <utility>
#include <walle/cortex/coroutine.hpp>

#include <stdexcept>

namespace walle::async {

namespace {

thread_local fiber_context_i* current_context = nullptr;

template <typename T>
struct scope_guard {
    scope_guard(T*& p, T* value)
        : ptr(p) {
        ptr = value;
    }

    ~scope_guard() {
        ptr = nullptr;
    }

    T*& ptr;
};

[[maybe_unused]] cortex::coroutine::flow_t make_coroutine_flow(fiber_t::flow_t flow) {
    return [fl = std::move(flow)]([[maybe_unused]] auto& self) mutable { fl(); };
}

} // namespace

fiber_t* fiber_t::make_fiber(exec::executor_i* executor, flow_t flow) {
    assert(executor);
    assert(!flow.empty());
    return new fiber_t(executor, std::move(flow));
}

fiber_t::fiber_t(exec::executor_i* executor, flow_t flow)
    : _executor(executor)
    , _suspend_context(nullptr)
    , _flow(std::move(flow))
    , _coroutine(cortex::coroutine::create([this](auto& self) {
        scope_guard<cortex::coroutine::suspend_context> guard(_suspend_context, &self);
        run();
    })) {}

void fiber_t::yield() {
    suspend();
}

void fiber_t::suspend() {
    assert(_suspend_context);
    _suspend_context->suspend();
}

void fiber_t::resume() {
    {
        scope_guard<fiber_context_i> sg(current_context, this);
        _coroutine.resume();

        if (_coroutine.is_done()) {
            destroy();
            return;
        }
    }

    schedule();
}

exec::executor_i* fiber_t::scheduler() const {
    return _executor;
}

void fiber_t::run() {
    _flow();
}

void fiber_t::destroy() {
    delete this;
}

void fiber_t::schedule() {
    assert(_executor);
    _executor->submit([this]() { resume(); });
}

fiber_context_i& fiber_t::self() {
    if (current_context == nullptr) {
        throw std::logic_error {"none fiber context"};
    }
    return *current_context;
}

} // namespace walle::async
