#include "walle/sync/wait_group.hpp"
#include <atomic>
#include <mutex>
#include <thread>

namespace walle::sync {

void basic_wait_group::add(std::size_t count) {
    const std::lock_guard lock(_mtx);
    _count += count;
}

void basic_wait_group::done(std::size_t count) {
    const std::lock_guard lock(_mtx);
    _count -= count;
    if (_count == 0) {
        _idle.notify_all();
    }
}

void basic_wait_group::wait() {
    std::unique_lock lock(_mtx);
    while (_count != 0) {
        _idle.wait(lock);
    }
}

void wait_group::add(std::size_t count) {
    _count.fetch_add(count, std::memory_order_relaxed);
}

void wait_group::done(std::size_t count) {
    if (_count.fetch_sub(count, std::memory_order_acq_rel) == count) {
        _idle.notify_all();
    }
}

void wait_group::wait() {
    while (_count.load(std::memory_order_acquire) != 0) {
        std::unique_lock lock(_mtx);
        _idle.wait(lock);
    }
}

} // namespace walle::sync
