#pragma once

#include <coroutine>
#include <exception>

namespace walle::std {

struct resumable {
    struct promise_type {
        using coroutine_handle = ::std::coroutine_handle<promise_type>;

        resumable get_return_object() {
            return resumable {coroutine_handle::from_promise(*this)};
        }
        auto initial_suspend() const noexcept {
            return ::std::suspend_always {};
        }
        auto final_suspend() const noexcept {
            return ::std::suspend_always {};
        }
        void return_void() {}
        void unhandled_exception() {
            ::std::terminate();
        }
    };

private:
    explicit resumable(promise_type::coroutine_handle handle) noexcept
        : _handle(handle) {}

public:
    bool resume() {
        if (_handle.done()) {
            return false;
        }
        _handle.resume();
        return true;
    }

    ~resumable() {
        if (!_handle.done()) {
            _handle.destroy();
        }
    }

private:
    promise_type::coroutine_handle _handle;
};

} // namespace walle::std
