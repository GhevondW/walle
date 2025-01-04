#pragma once

#include <cstddef>
#include <thread>
#include <vector>

#include <walle/core/mpmc_unbounded_blocking_queue.hpp>
#include <walle/core/non_copyable.hpp>
#include <walle/core/non_movable.hpp>
#include <walle/core/wait_group.hpp>
#include <walle/exec/executor.hpp>

namespace walle::exec {

class thread_pool final
    : core::non_copyable
    , core::non_movable
    , public executor_i {
public:
    explicit thread_pool(std::size_t workers_count);
    ~thread_pool() final = default;

    void submit(task_t task) final;

    // Maybe i need start function?
    // void start();
    void wait_idle();
    // Maybe I do not need this stop
    // i can do it in dtor, for now this is ok.
    void stop();

    [[nodiscard]] std::size_t workers_count() const {
        return _workers_count;
    }

private:
    void init();
    void loop();

    const std::size_t _workers_count;
    core::mpmc_unbounded_blocking_queue<task_t> _tasks;
    std::vector<std::thread> _workers;
    core::wait_group _wait_group;
};

} // namespace walle::exec
