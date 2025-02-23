#pragma once

#include <atomic>
#include <cassert>
#include <coroutine>
#include <cstddef>

#include <optional>
#include <type_traits>
#include <utility>
#include <walle/asymtx/oneway_task.hpp>
#include <walle/asymtx/scheduler.hpp>
#include <walle/asymtx/task.hpp>

#include <walle/core/defer.hpp>

namespace walle::asymtx {

namespace detail {

class async_task_handle_base_t {
public:
    async_task_handle_base_t()
        : _count(1u)
        , _continuation(nullptr) {}

protected:
    void on_work_finished() noexcept {
        if (_count.fetch_sub(1u, std::memory_order_acq_rel) == 1) {
            _continuation.resume();
        }
    }

    void on_work_started() noexcept {
        assert(_count.load(std::memory_order_relaxed) != 0);
        _count.fetch_add(1, std::memory_order_relaxed);
    }

protected:
    std::atomic<std::size_t> _count;
    std::coroutine_handle<> _continuation;
};

} // namespace detail

template <typename T>
class async_task_handle_t : public detail::async_task_handle_base_t {
private:
    template <typename ResultType>
    friend async_task_handle_t<ResultType> go(scheduler_t& scheduler, task_t<ResultType>&& task);

    async_task_handle_t(scheduler_t& scheduler, task_t<T>&& task)
        : async_task_handle_base_t() {
        start(scheduler, std::move(task));
    }

public:
    ~async_task_handle_t() = default;

    [[nodiscard]] auto get() noexcept {
        class awaiter {
            async_task_handle_t* _scope;

        public:
            awaiter(async_task_handle_t* scope) noexcept
                : _scope(scope) {}

            bool await_ready() noexcept {
                return _scope->_count.load(std::memory_order_acquire) == 0;
            }

            bool await_suspend(std::coroutine_handle<> continuation) noexcept {
                _scope->_continuation = continuation;
                return _scope->_count.fetch_sub(1u, std::memory_order_acq_rel) > 1u;
            }

            decltype(auto) await_resume() noexcept {
                return std::move(_scope->_value).value();
            }
        };

        return awaiter {this};
    }

private:
    void start(scheduler_t& scheduler, task_t<T>&& task) {
        [](async_task_handle_t* handle, scheduler_t& scheduler, task_t<T> task) -> oneway_task {
            handle->on_work_started();
            auto defer = core::defer_t([handle]() noexcept { handle->on_work_finished(); });
            co_await scheduler.schedule();
            handle->set_value(co_await std::move(task));
        }(this, scheduler, std::move(task));
    }

    template <typename... Args>
    void set_value(Args&&... args) {
        _value.emplace(std::forward<Args>(args)...);
    }

private:
    std::optional<T> _value;
};

template <>
class async_task_handle_t<void> : public detail::async_task_handle_base_t {
private:
    template <typename ResultType>
    friend async_task_handle_t<ResultType> go(scheduler_t& scheduler, task_t<ResultType>&& task);

    async_task_handle_t(scheduler_t& scheduler, task_t<void>&& task)
        : async_task_handle_base_t() {
        start(scheduler, std::move(task));
    }

public:
    ~async_task_handle_t() = default;

    [[nodiscard]] auto join() noexcept {
        class awaiter {
            async_task_handle_t* _scope;

        public:
            awaiter(async_task_handle_t* scope) noexcept
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
    void start(scheduler_t& scheduler, task_t<>&& task) {
        [](async_task_handle_t* handle, scheduler_t& scheduler, task_t<> task) -> oneway_task {
            handle->on_work_started();
            auto defer = core::defer_t([handle]() noexcept { handle->on_work_finished(); });
            co_await scheduler.schedule();
            co_await std::move(task);
        }(this, scheduler, std::move(task));
    }
};

template <typename T>
async_task_handle_t<T> go(scheduler_t& scheduler, task_t<T>&& task) {
    return async_task_handle_t<T> {scheduler, std::move(task)};
}

} // namespace walle::asymtx
