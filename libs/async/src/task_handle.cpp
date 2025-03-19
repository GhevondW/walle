#include "walle/async/task_handle.hpp"
#include "task_context.hpp"
#include "walle/async/error/error.hpp"

#include <cassert>
#include <cstddef>
#include <exception>

namespace walle::async {

task_handle::task_handle()
    : _impl(nullptr)
    , _event(nullptr) {}

task_handle::task_handle(task_handle&& other) noexcept
    : _impl(nullptr)
    , _event(nullptr) {
    std::swap(_impl, other._impl);
    std::swap(_event, other._event);
}

task_handle& task_handle::operator=(task_handle&& other) noexcept {
    if (joinable()) {
        std::terminate();
    }

    if (this == &other) {
        return *this;
    }

    std::swap(_impl, other._impl);
    std::swap(_event, other._event);
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
    throw error::not_implemeted_error_t {"not implemented yet."};
}

// none coroutine context
void task_handle::blocking_join() {
    assert(_impl);
    _event->wait();
    _impl = nullptr;
}

void task_handle::cancel() {
    throw error::not_implemeted_error_t {"not implemented yet."};
}

void task_handle::blocking_cancel() {
    throw error::not_implemeted_error_t {"not implemented yet."};
}

} // namespace walle::async
