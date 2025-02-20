#pragma once

#include <atomic>
#include <cassert>
#include <coroutine>
#include <exception>

#include <walle/asymtx/task.hpp>
#include <walle/core/defer.hpp>

namespace walle::asymtx {

// TODO : fix this design
class async_scope {
public:
    async_scope() noexcept
        : m_count(1u) {}

    ~async_scope() {
        // scope must be co_awaited before it destructs.
        assert(m_continuation);
    }

    void spawn(task_t<>&& task) {
        [](async_scope* scope, task_t<> task) -> oneway_task {
            scope->on_work_started();
            auto defer = core::defer_t([scope] { scope->on_work_finished(); });
            co_await std::move(task);
        }(this, std::move(task));
    }

    [[nodiscard]] auto join() noexcept {
        class awaiter {
            async_scope* m_scope;

        public:
            awaiter(async_scope* scope) noexcept
                : m_scope(scope) {}

            bool await_ready() noexcept {
                return m_scope->m_count.load(std::memory_order_acquire) == 0;
            }

            bool await_suspend(std::coroutine_handle<> continuation) noexcept {
                m_scope->m_continuation = continuation;
                return m_scope->m_count.fetch_sub(1u, std::memory_order_acq_rel) > 1u;
            }

            void await_resume() noexcept {}
        };

        return awaiter {this};
    }

private:
    void on_work_finished() noexcept {
        if (m_count.fetch_sub(1u, std::memory_order_acq_rel) == 1) {
            m_continuation.resume();
        }
    }

    void on_work_started() noexcept {
        assert(m_count.load(std::memory_order_relaxed) != 0);
        m_count.fetch_add(1, std::memory_order_relaxed);
    }

    struct oneway_task {
        struct promise_type {
            std::suspend_never initial_suspend() {
                return {};
            }
            std::suspend_never final_suspend() noexcept {
                return {};
            }
            void unhandled_exception() {
                std::terminate();
            }
            oneway_task get_return_object() {
                return {};
            }
            void return_void() {}
        };
    };

    std::atomic<size_t> m_count;
    std::coroutine_handle<> m_continuation;
};

} // namespace walle::asymtx
