#pragma once

#include <coroutine>
#include <cstddef>
#include <stdexcept>

namespace walle::asymtx {

class task_t;

namespace detail {

class task_promise_t {
private:
    struct final_awaitable {
        bool await_ready() const noexcept {
            return false;
        }

        template <typename TPromise>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<TPromise> coro) noexcept {
            return coro.promise()._continuation;
        }

        void await_resume() noexcept {}
    };

public:
    using coroutine_handle = std::coroutine_handle<task_promise_t>;

    task_promise_t() = default;

    inline auto get_return_object();

    std::suspend_always initial_suspend() noexcept {
        return {};
    }

    auto final_suspend() noexcept {
        return final_awaitable {};
    }

    void return_void() {}

    void unhandled_exception() {}

    void set_continuation(std::coroutine_handle<> continuation) noexcept {
        _continuation = continuation;
    }

private:
    std::coroutine_handle<> _continuation = nullptr;
};

} // namespace detail

// class promise;

class task_t {
private:
    using coroutine_handle = std::coroutine_handle<detail::task_promise_t>;

    struct awaitable {
        awaitable(coroutine_handle coroutine)
            : _coroutine(coroutine) {}

        bool await_ready() {
            return !_coroutine || _coroutine.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting_coroutine) {
            _coroutine.promise().set_continuation(awaiting_coroutine);
            return _coroutine;
        }

        void await_resume() {}

    private:
        coroutine_handle _coroutine;
    };

public:
    using promise_type = detail::task_promise_t;

    task_t()
        : _handle(nullptr) {}

    task_t(coroutine_handle handle) noexcept
        : _handle(handle) {}

    task_t(task_t&& other)
        : _handle(other._handle) {
        other._handle = nullptr;
    }

    task_t& operator=(task_t&& other) {
        if (&other != this) {
            if (_handle) {
                _handle.destroy();
            }
            _handle = other._handle;
            other._handle = nullptr;
        }

        return *this;
    }

    task_t(const task_t&) = delete;
    task_t& operator=(const task_t&) = delete;

    ~task_t() {
        if (_handle) {
            _handle.destroy();
        }
    }

    auto operator co_await() const& noexcept {
        return awaitable {_handle};
    }

private:
    void resume() {
        if (!_handle) {
            throw std::logic_error {"resume on invalid coroutine"};
        }

        if (_handle.done()) {
            throw std::logic_error {"resume on done coroutine"};
        }

        _handle.resume();
    }

private:
    coroutine_handle _handle;
};

namespace detail {

// will remove inline when do template
inline auto task_promise_t::get_return_object() {
    return task_t {coroutine_handle::from_promise(*this)};
}

} // namespace detail

} // namespace walle::asymtx
