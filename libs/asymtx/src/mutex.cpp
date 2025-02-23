#include <stdexcept>
#include <walle/asymtx/mutex.hpp>

namespace walle::asymtx {

namespace detail {

bool mutex_awaitable_t::await_ready() const {
    assert(_mutex);
    return false;
}

bool mutex_awaitable_t::await_suspend(std::coroutine_handle<> handle) noexcept {
    assert(_mutex);

    detail::mutex_awaitable_t* self = this;
    self->_awaiter = handle;

    detail::mutex_awaitable_t* old_head = _mutex->_head.load();
    while (true) {
        if (old_head == mutex_t::not_locked) {
            self->_next = mutex_t::not_locked;
            if (_mutex->_head.compare_exchange_weak(old_head, self)) {
                // do not suspend
                return false;
            }
        } else {
            self->_next = old_head;
            if (_mutex->_head.compare_exchange_weak(old_head, self)) {
                return true;
            }
        }
    }
}

void mutex_awaitable_t::await_resume() const noexcept {}

} // namespace detail

bool mutex_t::try_lock() {
    throw std::runtime_error {"Not implemented yet"};
}

void mutex_t::unlock() {
    assert(_head.load() != not_locked);

    auto current = _head.load();
    while (true) {
        auto next = current->_next;
        if (_head.compare_exchange_weak(current, next)) {
            if (next == not_locked) {
                return;
            } else {
                break;
            }
        }
    }

    current->_awaiter.resume();
}

} // namespace walle::asymtx
