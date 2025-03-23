#include "walle/async/task_handle.hpp"
#include "task_context.hpp"
#include "walle/async/error/error.hpp"

#include <cassert>
#include <exception>
#include <memory>

namespace walle::async {

task_handle_t::task_handle_t(std::shared_ptr<core::atomic_single_shot_event_t> event, task_context_t* task_context)
    : _event(std::move(event))
    , _impl(task_context) {}

task_handle_t::task_handle_t(task_handle_t&& other) noexcept
    : _event(nullptr)
    , _impl(nullptr) {
    std::swap(_impl, other._impl);
    std::swap(_event, other._event);
}

task_handle_t& task_handle_t::operator=(task_handle_t&& other) noexcept {
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

task_handle_t::~task_handle_t() {
    if (joinable()) {
        std::terminate();
    }
}

bool task_handle_t::is_valid() const noexcept {
    return _impl != nullptr;
}

bool task_handle_t::joinable() const noexcept {
    return _impl != nullptr;
}

task_id task_handle_t::get_id() const noexcept {
    // TODO : implement
    return 0;
}

// coroutine context
void task_handle_t::join() {
    assert(_impl);
    // TODO : do not use blocking join here, this is temporary
    _event->wait();
    _impl = nullptr;
}

// none coroutine context
void task_handle_t::blocking_join() {
    assert(_impl);
    _event->wait();
    _impl = nullptr;
}

void task_handle_t::cancel() {
    throw error::not_implemeted_error_t {"not implemented yet."};
}

void task_handle_t::blocking_cancel() {
    throw error::not_implemeted_error_t {"not implemented yet."};
}

} // namespace walle::async
