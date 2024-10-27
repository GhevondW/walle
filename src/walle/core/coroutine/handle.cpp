#include "walle/core/coroutine/handle.hpp"
#include "walle/core/coroutine/stack_allocator.hpp"
#include "walle/core/error.hpp"

// TODO : do not include a header from detail
#include <boost/context/detail/fcontext.hpp>

namespace walle::core::coroutine {

namespace {
namespace aux {

struct frame {
    frame(handle::flow_t flow, stack_allocator&& alloc, stack_allocator::stack_t stack)
        : _flow(std::move(flow))
        , _alloc(std::move(alloc))
        , _stack(stack) {}

    handle::flow_t _flow;
    stack_allocator _alloc;
    stack_allocator::stack_t _stack;
};

// template< typename Rec >
void frame_entry([[maybe_unused]] boost::context::detail::transfer_t t) noexcept {
    // transfer control structure to the context-stack
    // Rec * rec = static_cast< Rec * >( t.data);
    // BOOST_ASSERT( nullptr != t.fctx);
    // BOOST_ASSERT( nullptr != rec);
    // try {
    //     // jump back to `create_context()`
    //     t = jump_fcontext( t.fctx, nullptr);
    //     // start executing
    //     t.fctx = rec->run( t.fctx);
    // } catch ( forced_unwind const& ex) {
    //     t = { ex.fctx, nullptr };
    // }
    // BOOST_ASSERT( nullptr != t.fctx);
    // // destroy context-stack of `this`context on next context
    // ontop_fcontext( t.fctx, rec, fiber_exit< Rec >);
    // BOOST_ASSERT_MSG( false, "context already terminated");
}

} // namespace aux
} // namespace

struct handle::impl {
    boost::context::detail::fcontext_t _machine_context;
};

handle::handle(std::unique_ptr<typename handle::impl> impl) noexcept
    : _impl(std::move(impl)) {}

handle handle::create([[maybe_unused]] flow_t flow, [[maybe_unused]] stack_allocator&& alloc) {
    if (!alloc.is_valid()) {
        throw logic_error {"the stack allocator is invalid"};
    }
    auto stack = alloc.allocate();
    void* storage = reinterpret_cast<void*>(
        (reinterpret_cast<uintptr_t>(stack.top()) - static_cast<uintptr_t>(sizeof(aux::frame))) &
        ~static_cast<uintptr_t>(0xff));

    // placment new for control structure on context stack
    aux::frame* record = new (storage) aux::frame {std::move(flow), std::move(alloc), stack};
    // 64byte gab between control structure and stack top
    // should be 16byte aligned
    void* stack_top = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(storage) - static_cast<uintptr_t>(64));
    void* stack_bottom =
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(stack.top()) - static_cast<uintptr_t>(stack.size()));
    // create fast-context
    const std::size_t size = reinterpret_cast<uintptr_t>(stack_top) - reinterpret_cast<uintptr_t>(stack_bottom);

    const boost::context::detail::fcontext_t fctx = make_fcontext(stack_top, size, &aux::frame_entry);
    assert(fctx);
    // transfer control structure to context-stack
    return handle {std::unique_ptr<handle::impl>(
        new handle::impl {._machine_context = boost::context::detail::jump_fcontext(fctx, record).fctx})};
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
