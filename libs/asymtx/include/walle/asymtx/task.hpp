#pragma once

#include <cassert>
#include <coroutine>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <utility>
#include <variant>

#include <walle/core/overloaded.hpp>

namespace walle::asymtx {

template <typename ResultType>
class task_t;

namespace detail {

class task_promise_base_t {
private:
    struct final_awaitable {
        bool await_ready() const noexcept {
            return false;
        }

        template <typename TPromise>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<TPromise> coro) noexcept {
            return coro.promise()._continuation;
        }

        void await_resume() noexcept {
            // Should never get here.
            assert(false);
        }
    };

public:
    task_promise_base_t() = default;


    std::suspend_always initial_suspend() noexcept {
        return {};
    }

    auto final_suspend() noexcept {
        return final_awaitable {};
    }

    void set_continuation(std::coroutine_handle<> continuation) noexcept {
        _continuation = continuation;
    }

private:
    std::coroutine_handle<> _continuation = nullptr;
};

template <typename T>
class task_promise_t final : public task_promise_base_t {
public:
    using coroutine_handle = std::coroutine_handle<task_promise_t<T>>;

    task_promise_t()
        : _state(std::monostate {}) {}

    inline task_t<T> get_return_object() noexcept;

    template <typename... Args>
    void return_value(Args&&... args) {
        _state.template emplace<1>(std::forward<Args>(args)...);
    }

    void unhandled_exception() {
        _state.template emplace<2>(std::current_exception());
    }

    const T& result() const& {
        return std::visit(core::utils::overloaded_t {
                              [](std::monostate) -> const T& {
                                  // should never get here.
                                  throw std::runtime_error {"logic error std::monostate is not allowed"};
                              },
                              [](const T& value) -> const T& { return value; },
                              [](std::exception_ptr exception) -> const T& { std::rethrow_exception(exception); }},
                          _state);
    }

    // use after move is UB
    T result() && {
        return std::visit(
            core::utils::overloaded_t {[](std::monostate) -> T {
                                           // should never get here.
                                           throw std::runtime_error {"logic error std::monostate is not allowed"};
                                       },
                                       [](T& value) -> T { return std::move(value); },
                                       [](std::exception_ptr exception) -> T { std::rethrow_exception(exception); }},
            _state);
    }

private:
    std::variant<std::monostate, T, std::exception_ptr> _state;
};

template <>
class task_promise_t<void> final : public task_promise_base_t {
public:
    using coroutine_handle = std::coroutine_handle<task_promise_t<void>>;

    task_promise_t() = default;

    inline task_t<void> get_return_object() noexcept;

    void return_void() {}

    void unhandled_exception() {
        _exception = std::current_exception();
    }

    void result() {
        if (_exception) {
            std::rethrow_exception(_exception);
        }
    }

private:
    std::exception_ptr _exception = nullptr;
};

} // namespace detail

template <typename ResultType = void>
class task_t {
public:
    using result_type_t = ResultType;

private:
    using coroutine_handle = std::coroutine_handle<detail::task_promise_t<result_type_t>>;

    struct awaitable_base {
        awaitable_base(coroutine_handle coroutine)
            : _coroutine(coroutine) {}

        bool await_ready() {
            return !_coroutine || _coroutine.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting_coroutine) {
            _coroutine.promise().set_continuation(awaiting_coroutine);
            return _coroutine;
        }

    protected:
        coroutine_handle _coroutine;
    };

public:
    using promise_type = detail::task_promise_t<result_type_t>;

    task_t()
        : _handle(nullptr) {}

    task_t(coroutine_handle handle) noexcept
        : _handle(handle) {}

    task_t(task_t&& other)
        : _handle(other._handle) {
        other._handle = nullptr;
    }

    task_t& operator=(task_t&& other) {
        if (&other != this) {
            if (_handle) {
                _handle.destroy();
            }
            _handle = other._handle;
            other._handle = nullptr;
        }

        return *this;
    }

    task_t(const task_t&) = delete;
    task_t& operator=(const task_t&) = delete;

    ~task_t() {
        if (_handle) {
            _handle.destroy();
        }
    }

    auto operator co_await() const& noexcept {
        struct awaitable : public awaitable_base {
            using awaitable_base::awaitable_base;

            decltype(auto) await_resume() {
                if (this->_coroutine == nullptr) {
                    throw std::runtime_error {"broken task"};
                }
                return this->_coroutine.promise().result();
            }
        };

        return awaitable {_handle};
    }

    auto operator co_await() && noexcept {
        struct awaitable : public awaitable_base {
            using awaitable_base::awaitable_base;

            decltype(auto) await_resume() {
                if (this->_coroutine == nullptr) {
                    throw std::runtime_error {"broken task"};
                }
                return std::move(this->_coroutine.promise()).result();
            }
        };

        return awaitable {_handle};
    }

private:
    void resume() {
        if (!_handle) {
            throw std::logic_error {"resume on invalid coroutine"};
        }

        if (_handle.done()) {
            throw std::logic_error {"resume on done coroutine"};
        }

        _handle.resume();
    }

private:
    coroutine_handle _handle;
};

namespace detail {

template <typename T>
inline task_t<T> task_promise_t<T>::get_return_object() noexcept {
    return task_t<T> {coroutine_handle::from_promise(*this)};
}

inline task_t<void> task_promise_t<void>::get_return_object() noexcept {
    return task_t<void> {coroutine_handle::from_promise(*this)};
}

} // namespace detail

} // namespace walle::asymtx
