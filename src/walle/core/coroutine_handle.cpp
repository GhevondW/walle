#include "walle/core/coroutine_handle.hpp"
#include "walle/core/coroutine_stack_allocator.hpp"
#include "walle/core/error.hpp"

// TODO : do not include a header from detail
#include <boost/context/detail/fcontext.hpp>
#include <cassert>
#include <exception>
#include <memory>

#define ASSERT_HANDLE_INVARIANT \
    assert(_impl);              \
    assert(_impl->_machine_context);

namespace walle::core {

namespace {
namespace aux {

template <typename Impl>
struct suspend_fcontext : coroutine_handle::suspend_context_t {
    explicit suspend_fcontext(boost::context::detail::fcontext_t& fctx, Impl* impl) noexcept
        : _fctx(fctx)
        , _impl(impl) {}

    ~suspend_fcontext() noexcept override = default;

    void suspend() override {
        _fctx = boost::context::detail::jump_fcontext(_fctx, _impl).fctx;
    }

private:
    boost::context::detail::fcontext_t& _fctx;
    Impl* _impl;
};

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
    assert(transfer.fctx != nullptr);
    assert(frame != nullptr);

    suspend_fcontext suspender(transfer.fctx, impl);

    try {
        transfer = boost::context::detail::jump_fcontext(transfer.fctx, nullptr);
        impl->run(suspender);
    } catch (const forced_unwind& error) {
        transfer = {error.fctx, nullptr};
    } catch (const std::exception& error) {
        impl->_exception = std::current_exception();
    }

    assert(nullptr != t.fctx);
    boost::context::detail::ontop_fcontext(transfer.fctx, impl, frame_exit<Impl>);
    assert(false); // we must never get here.
}

} // namespace aux
} // namespace

struct coroutine_handle::impl {
    impl(coroutine_handle::flow_t flow, coroutine_stack_allocator alloc, coroutine_stack stack)
        : _is_done(false)
        , _machine_context(nullptr)
        , _flow(std::move(flow))
        , _alloc(alloc)
        , _stack(stack)
        , _exception(nullptr) {}

    void run(suspend_context_t& ctx) {
        _flow(ctx);
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

coroutine_handle::coroutine_handle(std::unique_ptr<typename coroutine_handle::impl> impl) noexcept
    : _impl(std::move(impl)) {}

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
    } catch (const std::exception& e) {
        alloc.deallocate(stack);
        throw;
    }

    auto* fctx =
        boost::context::detail::make_fcontext(stack.top(), stack.size(), &aux::frame_entry<coroutine_handle::impl>);
    assert(fctx != nullptr);

    impl->_machine_context = boost::context::detail::jump_fcontext(fctx, impl.get()).fctx;
    return coroutine_handle(std::move(impl));
}

coroutine_handle::~coroutine_handle() noexcept {
    if (_impl == nullptr || is_done()) {
        return;
    }

    boost::context::detail::ontop_fcontext(_impl->_machine_context, _impl.get(), aux::unwind);
}

coroutine_handle::coroutine_handle(coroutine_handle&& other) noexcept
    : _impl(std::move(other._impl)) {}

void coroutine_handle::resume() {
    ASSERT_HANDLE_INVARIANT;
    if (is_done()) {
        throw resume_on_completed_coroutine_error_t {"resume on finished coroutine"};
    }

    _impl->_machine_context = boost::context::detail::jump_fcontext(_impl->_machine_context, _impl.get()).fctx;

    if (_impl->_exception) {
        std::rethrow_exception(_impl->_exception);
    }
}

[[nodiscard]] bool coroutine_handle::is_done() const noexcept {
    ASSERT_HANDLE_INVARIANT;
    return _impl->_is_done;
}

} // namespace walle::core
