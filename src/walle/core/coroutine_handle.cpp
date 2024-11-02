#include "walle/core/coroutine_handle.hpp"
#include "walle/core/coroutine_stack_allocator.hpp"
#include "walle/core/error.hpp"

// TODO : do not include a header from detail
#include <boost/context/detail/fcontext.hpp>
#include <cassert>
#include <cstddef>
#include <memory>

namespace walle::core::coroutine {

namespace {
namespace aux {

struct forced_unwind {
    boost::context::detail::fcontext_t fctx {nullptr};

    forced_unwind() = default;

    explicit forced_unwind(boost::context::detail::fcontext_t fctx_)
        : fctx(fctx_) {}
};

struct frame {
    frame(coroutine_handle::flow_t flow, coroutine_stack_allocator&& alloc, coroutine_stack stack)
        : _flow(std::move(flow))
        , _alloc(std::move(alloc))
        , _stack(stack) {}

    void run([[maybe_unused]] boost::context::detail::fcontext_t ctx) {
        _flow();
    }

    void deallocate() noexcept {}

    coroutine_handle::flow_t _flow;
    coroutine_stack_allocator _alloc;
    coroutine_stack _stack;
};

template <typename Rec>
boost::context::detail::transfer_t fiber_exit(boost::context::detail::transfer_t t) noexcept {
    Rec* rec = static_cast<Rec*>(t.data);
    rec->deallocate();
    return {nullptr, nullptr};
}

template <typename Frame>
void frame_entry([[maybe_unused]] boost::context::detail::transfer_t t) noexcept {
    // transfer control structure to the context-stack
    auto frame = static_cast<Frame*>(t.data);
    assert(nullptr != t.fctx);
    assert(nullptr != frame);
    try {
        // jump back to `create_context()`
        t = boost::context::detail::jump_fcontext(t.fctx, nullptr);
        // start executing
        // t.fctx = frame->run(t.fctx);
    } catch (forced_unwind const& ex) {
        t = {ex.fctx, nullptr};
    }
    assert(nullptr != t.fctx);
    // destroy context-stack of `this`context on next context
    boost::context::detail::ontop_fcontext(t.fctx, frame, fiber_exit<Frame>);
    assert(false); // we must never get here.
}

} // namespace aux
} // namespace

struct coroutine_handle::impl {
    boost::context::detail::fcontext_t _machine_context;
};

coroutine_handle::coroutine_handle(std::unique_ptr<typename coroutine_handle::impl> impl) noexcept
    : _impl(std::move(impl)) {}

coroutine_handle coroutine_handle::create([[maybe_unused]] flow_t flow,
                                          [[maybe_unused]] coroutine_stack_allocator&& alloc) {
    if (!alloc.is_valid()) {
        throw logic_error {"the stack allocator is invalid"};
    }

    auto stack = alloc.allocate();

    void* storage = reinterpret_cast<void*>(
        (reinterpret_cast<std::size_t>(stack.top()) - static_cast<uintptr_t>(sizeof(aux::frame))) &
        ~static_cast<std::size_t>(0xff));

    auto* record = new (storage) aux::frame(std::move(flow), std::move(alloc), stack);

    void* stack_top = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(storage) - static_cast<std::size_t>(64));
    void* stack_bottom =
        reinterpret_cast<void*>(reinterpret_cast<std::size_t>(stack.top()) - static_cast<std::size_t>(stack.size()));
    // create fast-context
    const std::size_t size = reinterpret_cast<std::size_t>(stack_top) - reinterpret_cast<std::size_t>(stack_bottom);

    const boost::context::detail::fcontext_t fctx = make_fcontext(stack_top, size, &aux::frame_entry<aux::frame>);
    assert(fctx != nullptr);

    const auto result_fctx = boost::context::detail::jump_fcontext(fctx, record).fctx;
    return coroutine_handle(std::unique_ptr<coroutine_handle::impl>(new coroutine_handle::impl(result_fctx)));
}

coroutine_handle::~coroutine_handle() noexcept = default;

coroutine_handle::coroutine_handle(coroutine_handle&& other) noexcept
    : _impl(std::move(other._impl)) {}

void coroutine_handle::resume() {}

[[nodiscard]] bool coroutine_handle::is_done() const noexcept {
    // impl
    return false;
}

} // namespace walle::core::coroutine
