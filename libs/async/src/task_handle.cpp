#include "walle/async/task_handle.hpp"
#include "task_context.hpp"

#include <cassert>
#include <exception>

namespace walle::async {

task_handle::task_handle()
    : _impl() {}

task_handle::task_handle(task_handle&& other) noexcept
    : _impl() {
    _impl.swap(other._impl);
}

task_handle& task_handle::operator=(task_handle&& other) noexcept {
    if (joinable()) {
        std::terminate();
    }

    if (this == &other) {
        return *this;
    }

    _impl.swap(other._impl);
    return *this;
}

task_handle::~task_handle() {
    if (joinable()) {
        std::terminate();
    }
}

bool task_handle::is_valid() const noexcept {
    return _impl != nullptr;
}

bool task_handle::joinable() const noexcept {
    return _impl != nullptr;
}

task_id task_handle::get_id() const noexcept {
    // TODO : implement
    return 0;
}

// coroutine context
void task_handle::join() {
    assert(_impl);

    // Check id and joinable
    _impl->join();
    _impl.reset();
}

// none coroutine context
void task_handle::blocking_join() {
    assert(_impl);

    // Check id and joinable
    _impl->blocking_join();
    _impl.reset();
}

void task_handle::cancel() {}

void task_handle::blocking_cancel() {}

} // namespace walle::async
