#pragma once

#include <memory>

#include <walle/asymtx/scheduler.hpp>
#include <walle/asymtx/sync_task.hpp>

namespace walle::asymtx {

class sync_task_handle;
sync_task_handle sync_spawn(scheduler_t& scheduler, task_t<> task);

class sync_task_handle {
    friend sync_task_handle sync_spawn(scheduler_t& scheduler, task_t<> task);

    sync_task_handle(sync_task_t<>&& task);

public:
    sync_task_handle(sync_task_handle&& other);

    sync_task_handle& operator=(sync_task_handle&& other);

    ~sync_task_handle();

    void blocking_join();

private:
    void start();

private:
    std::unique_ptr<core::atomic_single_shot_event_t> _event;
    sync_task_t<> _task;
};

} // namespace walle::asymtx
