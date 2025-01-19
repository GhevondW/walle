#pragma once

#include <condition_variable>
#include <list>
#include <mutex>
#include <optional>

#include <walle/core/non_copyable.hpp>
#include <walle/core/non_movable.hpp>

namespace walle::core {

/**
 * @brief A thread-safe, unbounded blocking queue for multiple producers and multiple consumers.
 *
 * This queue supports the typical push and pop operations in a multithreaded environment.
 * The `push` operation inserts items into the queue, and the `wait_and_pop` operation blocks until
 * an item is available for popping. The queue can be closed using `wake_and_done`, which ensures that
 * consumers will not block indefinitely when the queue is empty and closed.
 *
 * @tparam T Type of the elements stored in the queue.
 */
template <typename T>
class mpmc_unbounded_blocking_queue
    : non_copyable
    , non_movable {
public:
    /**
     * @brief Default constructor.
     *
     * Initializes the queue in a ready-to-use state.
     */
    mpmc_unbounded_blocking_queue() noexcept = default;

    /**
     * @brief Destructor.
     *
     * Cleans up the queue, ensuring that all resources are freed.
     */
    ~mpmc_unbounded_blocking_queue() = default;

    /**
     * @brief Pushes a copy of an element onto the queue.
     *
     * This function adds the given element to the queue. If the queue is closed,
     * the operation fails and returns `false`.
     *
     * @param value The value to be pushed onto the queue.
     * @return `true` if the element was successfully pushed, `false` otherwise.
     */
    bool push(const T& value);

    /**
     * @brief Pushes a moved element onto the queue.
     *
     * This function adds the given rvalue element to the queue. If the queue is closed,
     * the operation fails and returns `false`.
     *
     * @param value The rvalue to be pushed onto the queue.
     * @return `true` if the element was successfully pushed, `false` otherwise.
     */
    bool push(T&& value);

    /**
     * @brief Waits for an element to be available and pops it from the queue.
     *
     * This function blocks until an element is available to be popped from the queue.
     * If the queue is closed and empty, it returns `std::nullopt`.
     *
     * @return An `std::optional<T>` containing the popped element, or `std::nullopt` if the queue is closed.
     */
    std::optional<T> wait_and_pop();

    /**
     * @brief Closes the queue, preventing further pushes.
     *
     * This function should be called before the queue is destroyed to ensure that no new elements
     * are pushed onto the queue after the close. It notifies all consumers waiting to pop from the queue.
     */
    void wake_and_done();

    /**
     * @brief Checks whether the queue is closed.
     *
     * This function checks if the queue is closed and no further elements can be pushed onto it.
     *
     * @return `true` if the queue is closed, `false` otherwise.
     */
    [[nodiscard]] bool is_closed() const;

private:
    /**
     * @brief Inserts a value into the queue.
     *
     * This is a helper function used by both `push` methods to insert elements into the queue.
     * If the queue is closed, the insertion fails.
     *
     * @tparam U Type of the value to be inserted.
     * @param value The value to be inserted.
     * @return `true` if the element was successfully inserted, `false` otherwise.
     */
    template <typename U>
    bool emplace(U&& value);

    mutable std::mutex _mtx {}; /**< Mutex for synchronizing access to the queue */
    std::condition_variable _cv {}; /**< Condition variable for waiting and notifying consumers */
    std::list<T> _queue {}; /**< The underlying list used to store the queue's elements */
    bool _closed = false; /**< Flag to indicate if the queue is closed */
};

template <typename T>
bool mpmc_unbounded_blocking_queue<T>::push(const T& value) {
    return emplace(value);
}

template <typename T>
bool mpmc_unbounded_blocking_queue<T>::push(T&& value) {
    return emplace(std::move(value));
}

template <typename T>
std::optional<T> mpmc_unbounded_blocking_queue<T>::wait_and_pop() {
    std::unique_lock lock(_mtx);
    while (_queue.empty() && !_closed) {
        _cv.wait(lock);
    }

    if (_queue.empty()) {
        return std::nullopt;
    }

    T data = std::move(_queue.front());
    _queue.pop_front();
    return data;
}

template <typename T>
void mpmc_unbounded_blocking_queue<T>::wake_and_done() {
    {
        std::lock_guard lock(_mtx);
        _closed = true;
    }
    _cv.notify_all();
}

template <typename T>
bool mpmc_unbounded_blocking_queue<T>::is_closed() const {
    std::lock_guard lock(_mtx);
    return _closed;
}

template <typename T>
template <typename U>
bool mpmc_unbounded_blocking_queue<T>::emplace(U&& value) {
    {
        std::lock_guard lock(_mtx);
        if (_closed) {
            return false;
        }
        _queue.emplace_back(std::forward<U>(value));
    }
    _cv.notify_one();
    return true;
}

} // namespace walle::core
