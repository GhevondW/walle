#pragma once

#include <atomic>
#include <cassert>
#include <coroutine>

#include <walle/asymtx/oneway_task.hpp>
#include <walle/asymtx/scheduler.hpp>
#include <walle/asymtx/task.hpp>

#include <walle/core/defer.hpp>

namespace walle::asymtx {

class async_scope_t {
public:
    async_scope_t() noexcept
        : _count(1u) {}

    ~async_scope_t() {
        // scope must be co_awaited before it destructs.
        assert(_continuation);
    }

    void schedule_on(scheduler_t& scheduler, task_t<>&& task) {
        [](async_scope_t* scope, scheduler_t& scheduler, task_t<> task) -> oneway_task {
            scope->on_work_started();
            auto defer = core::defer_t([scope]() noexcept { scope->on_work_finished(); });
            co_await scheduler.schedule();
            co_await std::move(task);
        }(this, scheduler, std::move(task));
    }

    [[nodiscard]] auto join() noexcept {
        class awaiter {
            async_scope_t* _scope;

        public:
            awaiter(async_scope_t* scope) noexcept
                : _scope(scope) {}

            bool await_ready() noexcept {
                return _scope->_count.load(std::memory_order_acquire) == 0;
            }

            bool await_suspend(std::coroutine_handle<> continuation) noexcept {
                _scope->_continuation = continuation;
                return _scope->_count.fetch_sub(1u, std::memory_order_acq_rel) > 1u;
            }

            void await_resume() noexcept {}
        };

        return awaiter {this};
    }

private:
    void on_work_finished() noexcept {
        if (_count.fetch_sub(1u, std::memory_order_acq_rel) == 1) {
            _continuation.resume();
        }
    }

    void on_work_started() noexcept {
        assert(_count.load(std::memory_order_relaxed) != 0);
        _count.fetch_add(1, std::memory_order_relaxed);
    }

    std::atomic<size_t> _count;
    std::coroutine_handle<> _continuation;
};

} // namespace walle::asymtx
