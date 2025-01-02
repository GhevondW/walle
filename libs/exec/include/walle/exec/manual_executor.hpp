#pragma once

#include <cstddef>
#include <queue>

#include <stdexcept>
#include <walle/exec/executor.hpp>

namespace walle::exec {

class manual_executor : public executor_i {
public:
    struct empty_executor : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    struct empty_task : std::logic_error {
        using std::logic_error::logic_error;
    };

    manual_executor() = default;
    ~manual_executor() override = default;

    void submit(task_t task) override;

    [[nodiscard]] std::size_t tasks_count() const noexcept;

    // might throw empty_executor
    // strong exception guarantee
    void run_one();

    // throws nothing if the queue is empty does nothing.
    // base exception guarantee
    void run_all();

private:
    std::queue<task_t> _tasks;
};

} // namespace walle::exec
