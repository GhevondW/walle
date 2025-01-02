#pragma once

#include <cstddef>
#include <thread>
#include <vector>

#include <walle/exec/executor.hpp>
#include <walle/core/mpmc_unbounded_blocking_queue.hpp>
#include <walle/core/wait_group.hpp>

namespace walle::exec {

class thread_pool final : public executor_i {
public:
    explicit thread_pool(std::size_t workers_count);
    ~thread_pool() final = default;

    thread_pool(const thread_pool& other) = delete;
    thread_pool(thread_pool&& other) noexcept = delete;
    thread_pool& operator=(const thread_pool& other) = delete;
    thread_pool& operator=(thread_pool&& other) noexcept = delete;

    void submit(task_t task) final;

    void wait_idle();
    // Maybe I do not need this stop
    // i can do ot in dtor, for now this is ok.
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
