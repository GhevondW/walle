#pragma once

#include <cassert>
#include <coroutine>
#include <stdexcept>

#include <walle/core/atomic_single_shot_event.hpp>

namespace walle::asymtx {

struct sync_task_promise;

class sync_task {
private:
    struct sync_task_promise;
    using coroutine_handle = std::coroutine_handle<sync_task_promise>;

    struct sync_task_promise {
    public:
        struct final_awaitable {
            explicit final_awaitable(core::atomic_single_shot_event_t* event) noexcept
                : _event(event) {
                assert(_event);
            }

            constexpr bool await_ready() const noexcept {
                return false;
            }
            constexpr void await_suspend(coroutine_handle handle) const noexcept {
                _event->set();
            }
            constexpr void await_resume() const noexcept {}

        private:
            core::atomic_single_shot_event_t* _event;
        };

        sync_task_promise() = default;

        sync_task get_return_object() {
            return sync_task {coroutine_handle::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept {
            return {};
        }

        auto final_suspend() noexcept {
            return final_awaitable {_event};
        }

        void return_void() {}

        void unhandled_exception() {}

        void set_event(core::atomic_single_shot_event_t* event) noexcept {
            _event = event;
        }

    private:
        core::atomic_single_shot_event_t* _event;
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

    void start(core::atomic_single_shot_event_t* event) {
        if (event == nullptr) {
            throw std::invalid_argument {"input event is null"};
        }
        _coro_handle.promise().set_event(event);
        resume();
    }

private:
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
