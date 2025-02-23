#pragma once

#include <cassert>
#include <memory>

#include <walle/asymtx/scheduler.hpp>
#include <walle/asymtx/sync_task.hpp>
#include <walle/asymtx/task.hpp>

namespace walle::asymtx {

template <typename T = void>
class sync_task_handle_t {
    template <typename ResultType>
    friend sync_task_handle_t<ResultType> sync_spawn(scheduler_t& scheduler, task_t<ResultType>&& task);

    template <typename ResultType>
    friend sync_task_handle_t<ResultType> sync_spawn(task_t<ResultType>&& task);

    sync_task_handle_t(sync_task_t<T>&& task);

public:
    sync_task_handle_t(sync_task_handle_t&& other);
    sync_task_handle_t& operator=(sync_task_handle_t&& other);
    ~sync_task_handle_t() = default;

    void blocking_join();
    T get() &&;

private:
    void start();

private:
    std::unique_ptr<core::atomic_single_shot_event_t> _event;
    sync_task_t<T> _task;
};

template <typename T>
inline sync_task_handle_t<T>::sync_task_handle_t(sync_task_t<T>&& task)
    : _event(std::make_unique<core::atomic_single_shot_event_t>())
    , _task(std::move(task)) {
    start();
}

template <typename T>
inline sync_task_handle_t<T>::sync_task_handle_t(sync_task_handle_t&& other)
    : _event(std::move(other._event))
    , _task(std::move(other._task)) {}

template <typename T>
inline sync_task_handle_t<T>& sync_task_handle_t<T>::operator=(sync_task_handle_t&& other) {
    if (this != &other) {
        _event = std::move(other._event);
        _task = std::move(other._task);
    }
    return *this;
}

template <typename T>
inline void sync_task_handle_t<T>::blocking_join() {
    assert(_event);
    _event->wait();
}

template <typename T>
inline T sync_task_handle_t<T>::get() && {
    blocking_join();
    return std::move(_task).detach();
}

template <typename T>
inline void sync_task_handle_t<T>::start() {
    assert(_event);
    _task.start(_event.get());
}

namespace detail {

template <typename T>
static sync_task_t<T> create_task_sync_task(scheduler_t& scheduler, task_t<T> task) {
    co_await scheduler.schedule();
    co_return co_await std::move(task);
}

template <typename T>
static sync_task_t<T> create_task_sync_task(task_t<T> task) {
    co_return co_await std::move(task);
}

} // namespace detail

template <typename T>
inline sync_task_handle_t<T> sync_spawn(scheduler_t& scheduler, task_t<T>&& task) {
    return sync_task_handle_t<T> {detail::create_task_sync_task(scheduler, std::move(task))};
}

template <typename T>
inline sync_task_handle_t<T> sync_spawn(task_t<T>&& task) {
    return sync_task_handle_t<T> {detail::create_task_sync_task(std::move(task))};
}

} // namespace walle::asymtx
