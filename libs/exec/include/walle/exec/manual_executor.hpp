#pragma once

#include <cstddef>
#include <queue>
#include <stdexcept>
#include <walle/core/non_copyable.hpp>
#include <walle/core/non_movable.hpp>
#include <walle/exec/executor.hpp>

namespace walle::exec {

/**
 * @brief A manual executor for executing tasks in a controlled manner.
 *
 * This executor allows manual execution of tasks that have been submitted.
 * Tasks are stored in a queue and can be executed one by one or all at once.
 */
class manual_executor
    : core::non_copyable
    , core::non_movable
    , public executor_i {
public:
    /**
     * @brief Exception thrown when attempting to run a task on an empty executor.
     */
    struct empty_executor : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    /**
     * @brief Exception thrown when attempting to submit an empty task.
     */
    struct empty_task : std::logic_error {
        using std::logic_error::logic_error;
    };

    manual_executor() = default;
    ~manual_executor() override = default;

    /**
     * @brief Submits a task for execution.
     * @param task The task to be executed.
     *
     * The task is stored in a queue and can be executed later using `run_one()` or `run_all()`.
     */
    void submit(task_t task) override;

    /**
     * @brief Returns the number of pending tasks in the executor.
     * @return The number of tasks in the queue.
     */
    [[nodiscard]] std::size_t tasks_count() const noexcept;

    /**
     * @brief Executes a single task from the queue.
     *
     * @throws empty_executor if there are no tasks in the queue.
     * @note Provides a strong exception guarantee.
     */
    void run_one();

    /**
     * @brief Executes all tasks in the queue.
     *
     * If the queue is empty, this function does nothing.
     * @note Provides a basic exception guarantee.
     */
    void run_all();

private:
    std::queue<task_t> _tasks; ///< Queue storing tasks to be executed.
};

} // namespace walle::exec
