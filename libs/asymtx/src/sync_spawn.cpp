#include <walle/asymtx/sync_spawn.hpp>
#include <walle/asymtx/task.hpp>

#include <cassert>

namespace walle::asymtx {

sync_task_handle::sync_task_handle(sync_task_t<>&& task)
    : _event(std::make_unique<core::atomic_single_shot_event_t>())
    , _task(std::move(task)) {
    start();
}

sync_task_handle::sync_task_handle(sync_task_handle&& other)
    : _event(std::move(other._event))
    , _task(std::move(other._task)) {}

sync_task_handle& sync_task_handle::operator=(sync_task_handle&& other) {
    if (this != &other) {
        _event = std::move(other._event);
        _task = std::move(other._task);
    }
    return *this;
}

sync_task_handle::~sync_task_handle() {
    assert(_task.is_done());
}

// must be called from none coroutine context
void sync_task_handle::blocking_join() {
    assert(_event);
    _event->wait();
}

void sync_task_handle::start() {
    assert(_event);
    _task.start(_event.get());
}

namespace {

static sync_task_t<> create_task_sync_task(scheduler_t& scheduler, task_t<> task) {
    co_await scheduler.schedule();
    co_await task;
    co_return;
}

} // namespace

sync_task_handle sync_spawn(scheduler_t& scheduler, task_t<> task) {
    return sync_task_handle {create_task_sync_task(scheduler, std::move(task))};
}

} // namespace walle::asymtx
