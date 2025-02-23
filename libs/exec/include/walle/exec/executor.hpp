#pragma once

#include <function2/function2.hpp>

namespace walle::exec {

/**
 * @brief Interface for an executor that schedules and runs tasks.
 *
 * This interface defines an abstract executor that allows submitting tasks for execution.
 * Implementations of this interface must define how tasks are scheduled and executed.
 */
struct executor_i {
    using task_t = fu2::unique_function<void()>; ///< Alias for a unique function representing a task.

    executor_i() = default;

    virtual ~executor_i() = default;

    /**
     * @brief Submits a task for execution.
     * @param task The task to be executed.
     *
     * The task is a callable object that encapsulates a unit of work.
     * Implementations must define how tasks are scheduled and run.
     */
    virtual void submit(task_t task) = 0;

    // Uncomment if needed
    // /**
    //  * @brief Stops the executor.
    //  *
    //  * This function should stop all task processing and prevent new tasks from being submitted.
    //  */
    // virtual void stop() = 0;
};

} // namespace walle::exec
