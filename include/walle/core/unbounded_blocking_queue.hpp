#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

#include <walle/core/error.hpp>

namespace walle::core {

template <typename T>
class unbounded_blocking_queue {
public:
    struct operation_on_closed_queue : error {
        using error::error;
    };

    unbounded_blocking_queue() = default;
    ~unbounded_blocking_queue() noexcept;
    unbounded_blocking_queue(const unbounded_blocking_queue&) = delete;
    unbounded_blocking_queue(unbounded_blocking_queue&&) noexcept = delete;
    unbounded_blocking_queue& operator=(const unbounded_blocking_queue&) = delete;
    unbounded_blocking_queue& operator=(unbounded_blocking_queue&&) noexcept = delete;

    void push(T value);
    std::optional<T> try_pop();
    T wait_and_pop();
    void wake_and_done() noexcept;

    [[nodiscard]] bool is_done() const {
        std::lock_guard lock {_mtx};
        return _done;
    }

private:
    std::queue<T> _queue {};
    mutable std::mutex _mtx {};
    std::condition_variable _cv {};
    bool _done {};
};

template <typename T>
unbounded_blocking_queue<T>::~unbounded_blocking_queue() noexcept {
    wake_and_done();
}

template <typename T>
void unbounded_blocking_queue<T>::push(T value) {
    {
        std::lock_guard lock(_mtx);
        if (_done) {
            throw operation_on_closed_queue {"the channel is closed"};
        }

        _queue.push(std::move(value));
    }
    _cv.notify_one();
}

template <typename T>
std::optional<T> unbounded_blocking_queue<T>::try_pop() {
    std::lock_guard lock(_mtx);
    if (_done) {
        throw operation_on_closed_queue {"the channel is closed"};
    }

    if (_queue.empty()) {
        return std::nullopt;
    }

    std::optional<T> opt(std::move(_queue.front()));
    _queue.pop();
    return opt;
}

template <typename T>
T unbounded_blocking_queue<T>::wait_and_pop() {
    std::unique_lock lock(_mtx);
    while (_queue.empty() && !_done) {
        _cv.wait(lock);
    }

    if (_done) {
        throw operation_on_closed_queue {"the channel is closed"};
    }

    T res(std::move(_queue.front()));
    _queue.pop();
    return res;
}

template <typename T>
void unbounded_blocking_queue<T>::wake_and_done() noexcept {
    {
        std::lock_guard lg(_mtx);
        _done = true;
    }
    _cv.notify_all();
}

} // namespace walle::core
