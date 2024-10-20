#include <assert.h>
#include <cstddef>
#include <cstdlib>
#include <memory>

#include <walle/coro/coroutine.hpp>

#include <walle/core/error.hpp>

#include <boost/context/detail/fcontext.hpp>

namespace walle::coro {

struct coroutine::impl {
    impl(std::size_t stack_size, void* mem, coroutine::flow_t flow, boost::context::detail::fcontext_t ctx)
        : _stack_size(stack_size)
        , _mem(mem)
        , _flow(std::move(flow))
        , _fcontext(ctx) {}

    std::size_t _stack_size;
    void* _mem;
    coroutine::flow_t _flow;
    boost::context::detail::fcontext_t _fcontext;

    void run(suspend_context_t context) {
        _flow(context);
    }

    ~impl() {
        free(_mem);
    }
};

namespace aux {

static void coro_entry(boost::context::detail::transfer_t t) noexcept {
    auto* impl = static_cast<coroutine::impl*>(t.data);
    assert(t.fctx != nullptr);
    assert(impl != nullptr);
    // try {
    // jump back to `create_context()`
    t = boost::context::detail::jump_fcontext(t.fctx, nullptr);
    // start executing
    impl->run(coroutine::suspend_context_t {reinterpret_cast<void*>(t.fctx)});
    // } catch ( forced_unwind const& ex) {
    //     t = { ex.fctx, nullptr };
    // }

    assert(nullptr != t.fctx);
    // destroy context-stack of `this`context on next context
    // ontop_fcontext( t.fctx, rec, fiber_exit< Rec >);
    assert(false);
};

} // namespace aux

void coroutine::suspend_context_t::suspend() {
    auto fctx = reinterpret_cast<boost::context::detail::fcontext_t>(_cnt);
    boost::context::detail::jump_fcontext(fctx, nullptr);
}

coroutine coroutine::create(flow_t flow, std::size_t stack_size) {
    if (stack_size == 0) {
        throw core::logic_error {"the stack size is 0"};
    }

    if (flow.empty()) {
        throw core::logic_error {"the flow is empty"};
    }

    std::byte* mem = reinterpret_cast<std::byte*>(malloc(stack_size));
    if (mem == nullptr) {
        throw coroutine::stack_allocation_error_t {"unable to allocate a stack for coroutine"};
    }

    std::byte* stack_top = mem + stack_size;

    auto impl = std::make_unique<coroutine::impl>(stack_size, mem, std::move(flow), nullptr);

    const auto fctx = make_fcontext(stack_top, stack_size, &aux::coro_entry);
    assert(fctx != nullptr);

    impl->_fcontext = boost::context::detail::jump_fcontext(fctx, impl.get()).fctx;

    return coroutine(std::move(impl));
}

coroutine::coroutine(std::unique_ptr<impl> impl) noexcept
    : _impl(std::move(impl)) {}

coroutine::~coroutine() noexcept = default;

coroutine::coroutine([[maybe_unused]] coroutine&& other) noexcept {
    assert(false);
}

void coroutine::resume() {
    boost::context::detail::jump_fcontext(_impl->_fcontext, nullptr);
}

} // namespace walle::coro
