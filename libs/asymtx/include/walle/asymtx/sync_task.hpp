#pragma once

#include <cassert>
#include <coroutine>
#include <exception>
#include <optional>
#include <stdexcept>

#include <type_traits>
#include <utility>
#include <walle/core/atomic_single_shot_event.hpp>

namespace walle::asymtx {

namespace detail {

struct sync_task_final_awaitable {
    explicit sync_task_final_awaitable(core::atomic_single_shot_event_t* event) noexcept
        : _event(event) {
        assert(_event);
    }

    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) const noexcept {
        _event->set();
    }

    void await_resume() const noexcept {}

private:
    core::atomic_single_shot_event_t* _event;
};

} // namespace detail

template <typename ResultValue = void>
class sync_task {
private:
    struct sync_task_promise_base {
    public:
        sync_task_promise_base() noexcept
            : _event(nullptr)
            , _exception(nullptr) {}

        std::suspend_always initial_suspend() noexcept {
            return {};
        }

        auto final_suspend() noexcept {
            return detail::sync_task_final_awaitable {_event};
        }

        void unhandled_exception() noexcept {
            _exception = std::current_exception();
        }

        void set_event(core::atomic_single_shot_event_t* event) noexcept {
            _event = event;
        }

        bool has_exception() const noexcept {
            return _exception != nullptr;
        }

        std::exception_ptr exception() const noexcept {
            return _exception;
        }

    private:
        core::atomic_single_shot_event_t* _event = nullptr;
        std::exception_ptr _exception = nullptr;
    };

    template <typename PResultValue>
    struct sync_task_promise : public sync_task_promise_base {
        using coroutine_handle_t = std::coroutine_handle<sync_task_promise<ResultValue>>;

    public:
        sync_task_promise() = default;

        sync_task get_return_object() {
            return sync_task {coroutine_handle_t::from_promise(*this)};
        }

        template <typename... Args>
        void return_value(Args&&... args) {
            _result.emplace(std::forward<Args>(args)...);
        }

        std::optional<PResultValue> get_result() noexcept {
            return std::move(_result);
        }

    private:
        std::optional<PResultValue> _result;
    };

    template <>
    struct sync_task_promise<void> : public sync_task_promise_base {
        using coroutine_handle_t = std::coroutine_handle<sync_task_promise<void>>;

    public:
        sync_task_promise() = default;

        sync_task get_return_object() {
            return sync_task {coroutine_handle_t::from_promise(*this)};
        }

        void return_void() {}
    };

    using coroutine_handle_t = std::coroutine_handle<sync_task_promise<ResultValue>>;

    explicit sync_task()
        : _coro_handle(nullptr) {}

    explicit sync_task(coroutine_handle_t coro_handle)
        : _coro_handle(coro_handle) {}

public:
    using promise_type = sync_task_promise<ResultValue>;

    sync_task(const sync_task&) = delete;
    sync_task& operator=(const sync_task&) = delete;

    sync_task(sync_task&& other)
        : _coro_handle(other._coro_handle) {
        other._coro_handle = nullptr;
    }

    sync_task& operator=(sync_task&& task) {
        if (&task != this) {
            if (_coro_handle) {
                _coro_handle.destroy();
            }

            _coro_handle = task._coro_handle;
            task._coro_handle = nullptr;
        }

        return *this;
    }

    ~sync_task() {
        if (_coro_handle) {
            _coro_handle.destroy();
        }
    }

    bool is_valid() const noexcept {
        return static_cast<bool>(_coro_handle);
    }

    bool is_done() const noexcept {
        return _coro_handle.done();
    }

    void start(core::atomic_single_shot_event_t* event) {
        if (event == nullptr) {
            throw std::invalid_argument {"Event argument cannot be null"};
        }
        _coro_handle.promise().set_event(event);
        resume();
    }

    // Returns the result or rethrows an uncaught exception
    ResultValue detach() && {
        if (!_coro_handle) {
            throw std::logic_error {"detach() called on an invalid task"};
        }

        if (!_coro_handle.done()) {
            throw std::logic_error {"detach() called on an unfinished task"};
        }

        sync_task self;
        std::swap(*this, self);

        auto& promise = self.promise();
        if (promise.has_exception()) {
            std::rethrow_exception(promise.exception());
        }

        if constexpr (std::is_same_v<ResultValue, void>) {
            return;
        } else {
            auto result_opt = promise.get_result();
            assert(result_opt.has_value());
            return std::move(result_opt).value();
        }
    }

private:
    void resume() {
        if (!_coro_handle) {
            throw std::logic_error {"resume() called on an invalid coroutine"};
        }

        if (_coro_handle.done()) {
            throw std::logic_error {"resume() called on a finished coroutine"};
        }

        _coro_handle.resume();
    }

    decltype(auto) promise() {
        return _coro_handle.promise();
    }

private:
    coroutine_handle_t _coro_handle;
};

} // namespace walle::asymtx
