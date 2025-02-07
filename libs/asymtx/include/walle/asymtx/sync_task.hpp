#pragma once

#include <coroutine>
#include <stdexcept>

namespace walle::asymtx {

struct sync_task_promise;

class sync_task {
private:
    struct sync_task_promise;
    using coroutine_handle = std::coroutine_handle<sync_task_promise>;

    struct sync_task_promise {
        sync_task get_return_object() {
            return sync_task {coroutine_handle::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept {
            return {};
        }
        std::suspend_always final_suspend() noexcept {
            return {};
        }
        void return_void() {}
        void unhandled_exception() {}
    };

    explicit sync_task()
        : _coro_handle(nullptr) {}

    explicit sync_task(coroutine_handle coro_handle)
        : _coro_handle(coro_handle) {}

public:
    using promise_type = sync_task_promise;

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

    void resume() {
        if (!_coro_handle) {
            throw std::logic_error {"resume on invalid coroutine"};
        }

        if (_coro_handle.done()) {
            throw std::logic_error {"resume on finished coroutine"};
        }

        _coro_handle.resume();
    }

private:
    coroutine_handle _coro_handle;
};

} // namespace walle::asymtx
