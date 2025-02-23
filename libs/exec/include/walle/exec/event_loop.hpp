#pragma once

#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>

#include <walle/core/non_copyable.hpp>
#include <walle/core/non_movable.hpp>
#include <walle/exec/executor.hpp>

namespace walle::exec {

/**
 * @brief A single-threaded event loop executor.
 *
 * This executor processes submitted tasks in a dedicated worker thread.
 * Tasks are executed sequentially in the order they are submitted.
 */
class event_loop
    : core::non_copyable
    , core::non_movable
    , public std::enable_shared_from_this<event_loop>
    , public executor_i {
private:
    /**
     * @brief Private constructor tag to enforce controlled instantiation.
     */
    struct private_t {
        explicit private_t() = default;
    };

public:
    /**
     * @brief Constructs an event loop.
     * @param private_t A private tag to restrict direct instantiation.
     *
     * Use `make()` to create an instance.
     */
    event_loop(private_t);

    /**
     * @brief Creates and returns a shared instance of event_loop.
     * @return A shared pointer to a new event_loop instance.
     */
    static std::shared_ptr<event_loop> make();

    ~event_loop() override;

    /**
     * @brief Submits a task to the event loop.
     * @param task The task to be executed.
     *
     * The task will be stored in a queue and executed by the event loop thread.
     */
    void submit(task_t task) override;

    /**
     * @brief Stops the event loop.
     *
     * Signals the worker thread to exit and processes any remaining tasks.
     * Must be calles before destructor
     */
    void stop();

private:
    /**
     * @brief The main loop that processes submitted tasks.
     *
     * This function runs in the worker thread and waits for new tasks.
     */
    void loop();

    std::thread _worker; ///< The worker thread executing tasks.
    std::list<task_t> _tasks; ///< Task queue, guarded by `_mtx`.
    bool _done; ///< Flag indicating whether the event loop should stop.
    std::mutex _mtx; ///< Mutex to guard access to `_tasks`.
    std::condition_variable _cv; ///< Condition variable for task synchronization.
};

} // namespace walle::exec
