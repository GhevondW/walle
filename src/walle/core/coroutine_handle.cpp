#include "walle/core/coroutine_handle.hpp"
#include "walle/core/coroutine_stack_allocator.hpp"
#include "walle/core/error.hpp"

// TODO : do not include a header from detail
#define BOOST_USE_ASAN
#include <boost/context/detail/fcontext.hpp>
#include <cassert>
#include <exception>
#include <memory>

namespace walle::core {

namespace {
namespace aux {

// template <typename Impl>
// struct suspend_fcontext : coroutine_handle::suspend_context_t {
//     explicit suspend_fcontext(boost::context::detail::fcontext_t fctx, Impl* impl) noexcept
//         : _fctx(fctx)
//         , _impl(impl) {}

//     suspend_fcontext(const suspend_fcontext&) = delete;
//     suspend_fcontext(suspend_fcontext&&) noexcept = delete;
//     suspend_fcontext& operator=(const suspend_fcontext&) = delete;
//     suspend_fcontext& operator=(suspend_fcontext&&) noexcept = delete;

//     ~suspend_fcontext() override = default;

//     void suspend() override {
//         if(_fctx == nullptr) {
//             std::terminate();
//         }

//         _fctx = boost::context::detail::jump_fcontext(_fctx, _impl).fctx;

//         if(_fctx == nullptr) {
//             std::terminate();
//         }
//     }

// private:
//     boost::context::detail::fcontext_t _fctx;
//     Impl* _impl;
// };

struct forced_unwind : std::exception {
    boost::context::detail::fcontext_t fctx {nullptr};

    explicit forced_unwind(boost::context::detail::fcontext_t fctx_)
        : fctx(fctx_) {}

    [[nodiscard]] const char* what() const noexcept override {
        return nullptr;
    }
};

boost::context::detail::transfer_t unwind(boost::context::detail::transfer_t transfer) {
    throw forced_unwind(transfer.fctx);
}

template <typename Impl>
boost::context::detail::transfer_t frame_exit(boost::context::detail::transfer_t transfer) noexcept {
    auto impl = static_cast<Impl*>(transfer.data);
    impl->deallocate_stack();
    impl->_is_done = true;
    return {nullptr, nullptr};
}

template <typename Impl>
void frame_entry(boost::context::detail::transfer_t transfer) noexcept {
    auto impl = static_cast<Impl*>(transfer.data);
    {
        assert(transfer.fctx != nullptr);
        assert(impl != nullptr);

        try {
            transfer = boost::context::detail::jump_fcontext(transfer.fctx, nullptr);
            coroutine_handle::suspend_context suspender(transfer.fctx, impl);
            transfer.fctx = impl->run(suspender);
        } catch (const forced_unwind& error) {
            transfer = {error.fctx, nullptr};
        } catch (const std::exception&) {
            impl->_exception = std::current_exception();
        }

        assert(nullptr != transfer.fctx);
    }
    boost::context::detail::ontop_fcontext(transfer.fctx, impl, frame_exit<Impl>);
    assert(false); // we must never get here.
}

} // namespace aux
} // namespace

void coroutine_handle::suspend_context::suspend() {
    if(_context == nullptr) {
        std::terminate();
    }

    _context = boost::context::detail::jump_fcontext(_context, _impl).fctx;

    if(_context == nullptr) {
        std::terminate();
    }
}

struct coroutine_handle::impl {
    impl(coroutine_handle::flow_t flow, coroutine_stack_allocator alloc, coroutine_stack stack)
        : _is_done(false)
        , _machine_context(nullptr)
        , _flow(std::move(flow))
        , _alloc(alloc)
        , _stack(stack)
        , _exception(nullptr) {}

    void* run(suspend_context ctx) {
        _flow(ctx);
        return ctx._context;
    }

    void deallocate_stack() noexcept {
        _alloc.deallocate(_stack);
    }

    bool _is_done {};
    boost::context::detail::fcontext_t _machine_context {};
    coroutine_handle::flow_t _flow {};
    coroutine_stack_allocator _alloc {};
    coroutine_stack _stack {};
    std::exception_ptr _exception {};
};

coroutine_handle::coroutine_handle(std::unique_ptr<typename coroutine_handle::impl> in_impl) noexcept
    : _impl(std::move(in_impl)) {}

coroutine_handle coroutine_handle::create(flow_t flow, coroutine_stack_allocator&& alloc) {
    if (flow.empty()) {
        throw std::logic_error {"the flow is empty."};
    }

    if (!alloc.is_valid()) {
        throw std::logic_error {"the stack allocator is invalid."};
    }

    auto stack = alloc.allocate();
    std::unique_ptr<coroutine_handle::impl> impl;
    try {
        impl = std::make_unique<coroutine_handle::impl>(std::move(flow), alloc, stack);
    } catch (const std::exception&) {
        alloc.deallocate(stack);
        throw;
    }

    // void* stack_top = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(stack.top()) - static_cast<uintptr_t>(64));
    // void* stack_bottom =
    //     reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(stack.top()) - static_cast<uintptr_t>(stack.size()));
    // // create fast-context
    // const std::size_t size = reinterpret_cast<uintptr_t>(stack_top) - reinterpret_cast<uintptr_t>(stack_bottom);

    auto* fctx = boost::context::detail::make_fcontext(stack.top(), stack.size(), &aux::frame_entry<coroutine_handle::impl>);
    assert(fctx != nullptr);

    impl->_machine_context = boost::context::detail::jump_fcontext(fctx, impl.get()).fctx;
    return coroutine_handle(std::move(impl));
}

coroutine_handle::~coroutine_handle() {
    if (_impl == nullptr || is_done()) {
        return;
    }

    boost::context::detail::ontop_fcontext(_impl->_machine_context, _impl.get(), aux::unwind);
}

coroutine_handle::coroutine_handle(coroutine_handle&& other) noexcept
    : _impl(std::move(other._impl)) {}

void coroutine_handle::resume() {
    assert(_impl);
    if (is_done()) {
        throw resume_on_completed_coroutine_error_t {"resume on finished coroutine"};
    }

    _impl->_machine_context = boost::context::detail::jump_fcontext(_impl->_machine_context, _impl.get()).fctx;

    if (_impl->_exception) {
        std::rethrow_exception(_impl->_exception);
    }
}

[[nodiscard]] bool coroutine_handle::is_done() const noexcept {
    assert(_impl);
    return _impl->_is_done;
}

} // namespace walle::core
