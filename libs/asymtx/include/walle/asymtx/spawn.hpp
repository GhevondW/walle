#pragma once

#include <cassert>
#include <utility>
#include <walle/asymtx/scheduler.hpp>
#include <walle/asymtx/sync_task.hpp>
#include <walle/asymtx/task.hpp>
#include <walle/core/single_shot_event.hpp>

namespace walle::asymtx {

class task_handle;
static task_handle spawn(scheduler_t& scheduler, task_t<> task);

class task_handle {
    friend task_handle spawn(scheduler_t& scheduler, task_t<> task);

    task_handle(sync_task_t<>&& task)
        : _event(std::make_unique<core::atomic_single_shot_event_t>())
        , _task(std::move(task)) {
        start();
    }

public:
    task_handle(task_handle&& other)
        : _event(std::move(other._event))
        , _task(std::move(other._task)) {}

    task_handle& operator=(task_handle&& other) {
        if (this != &other) {
            _event = std::move(other._event);
            _task = std::move(other._task);
        }
        return *this;
    }

    ~task_handle() {
        assert(_task.is_done());
    }

    // must be called from none coroutine context
    void blocking_join() {
        assert(_event);
        _event->wait();
    }

private:
    void start() {
        assert(_event);
        _task.start(_event.get());
    }

private:
    std::unique_ptr<core::atomic_single_shot_event_t> _event;
    sync_task_t<> _task;
};

namespace detail {

static sync_task_t<> create_task_sync_task(scheduler_t& scheduler, task_t<> task) {
    co_await scheduler.schedule();
    co_await task;
    co_return;
}

} // namespace detail

static task_handle spawn(scheduler_t& scheduler, task_t<> task) {
    return task_handle {detail::create_task_sync_task(scheduler, std::move(task))};
}

} // namespace walle::asymtx
