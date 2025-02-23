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

/**
 * @brief A thread pool executor for running tasks concurrently.
 *
 * This thread pool manages a fixed number of worker threads that execute submitted tasks.
 */
class thread_pool final
    : core::non_copyable
    , core::non_movable
    , public executor_i {
public:
    /**
     * @brief Constructs a thread pool with a given number of worker threads.
     * @param workers_count The number of threads in the pool.
     */
    explicit thread_pool(std::size_t workers_count);

    ~thread_pool() final = default;

    /**
     * @brief Submits a task to the thread pool.
     * @param task The task to be executed asynchronously.
     *
     * The task is added to a queue and will be executed by an available worker thread.
     */
    void submit(task_t task) final;

    /**
     * @brief Waits until all submitted tasks have been processed.
     *
     * This function blocks until the queue is empty and all tasks are completed.
     */
    void wait_idle();

    /**
     * @brief Stops the thread pool and joins all worker threads.
     *
     * This function ensures that all workers are properly shut down.
     * Must be called befor destructor
     */
    void stop();

    /**
     * @brief Gets the number of worker threads.
     * @return The number of worker threads in the pool.
     */
    [[nodiscard]] std::size_t workers_count() const {
        return _workers_count;
    }

private:
    /**
     * @brief Initializes the worker threads and starts execution loops.
     */
    void init();

    /**
     * @brief Worker thread loop that continuously processes tasks.
     */
    void loop();

    const std::size_t _workers_count; ///< Number of worker threads.
    core::mpmc_unbounded_blocking_queue<task_t> _tasks; ///< Queue of tasks to be executed.
    std::vector<std::thread> _workers; ///< Vector of worker threads.
    core::wait_group _wait_group; ///< Synchronization primitive for tracking active tasks.
};

} // namespace walle::exec
