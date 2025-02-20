#pragma once

#include <atomic>
#include <cassert>
#include <coroutine>
#include <stdexcept>

namespace walle::asymtx {

class mutex_t;

namespace detail {

struct mutex_awaitable_t {
    mutex_awaitable_t(mutex_t* mutex)
        : _mutex(mutex)
        , _next(nullptr)
        , _awaiter(nullptr) {
        assert(_mutex);
    }

    bool await_ready() const;

    bool await_suspend(std::coroutine_handle<> handle) noexcept;

    void await_resume() const noexcept;

    mutex_t* _mutex = nullptr;
    mutex_awaitable_t* _next = nullptr;
    std::coroutine_handle<> _awaiter = nullptr;
};

} // namespace detail

class mutex_t {
    friend struct detail::mutex_awaitable_t;

public:
    mutex_t()
        : _head(not_locked) {}

    ~mutex_t() {
        assert(_head.load() == not_locked);
    }

    detail::mutex_awaitable_t lock() {
        return {this};
    }

    bool try_lock() {
        throw std::runtime_error {"Not implemented yet"};
    }

    void unlock();

private:
    static constexpr detail::mutex_awaitable_t* not_locked = nullptr;

    std::atomic<detail::mutex_awaitable_t*> _head;
};

} // namespace walle::asymtx
