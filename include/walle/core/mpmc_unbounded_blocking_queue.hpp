#pragma once

#include <list>
#include <optional>
#include <mutex>
#include <condition_variable>

namespace walle::core {

template <typename T>
class mpmc_unbounded_blocking_queue {
public:
    mpmc_unbounded_blocking_queue() noexcept = default;
    mpmc_unbounded_blocking_queue(const mpmc_unbounded_blocking_queue&) = delete;
    mpmc_unbounded_blocking_queue(mpmc_unbounded_blocking_queue&&) noexcept = delete;
    mpmc_unbounded_blocking_queue& operator=(const mpmc_unbounded_blocking_queue&) = delete;
    mpmc_unbounded_blocking_queue& operator=(mpmc_unbounded_blocking_queue&&) noexcept = delete;
    ~mpmc_unbounded_blocking_queue() = default;

    bool push(const T& value);
    bool push(T&& value);

    // Returns nullopt if queue is closed
    std::optional<T> wait_and_pop();

    // closes the queue, must be called before destruction
    void wake_and_done();

    // Used for testing
    [[nodiscard]] bool is_closed() const;

private:
    template <typename U>
    bool emplace(U&& value);

    mutable std::mutex _mtx {};
    std::condition_variable _cv {};
    std::list<T> _queue {};
    bool _closed = false;
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
