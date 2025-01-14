#pragma once

#include "walle/cortex/suspend_context.hpp"
#include <cassert>
#include <stdexcept>
#include <utility>
#include <walle/cortex/coroutine.hpp>

#include <memory>
#include <optional>
#include <type_traits>

#include <function2/function2.hpp>

namespace walle::cortex {

template <typename ResultType>
class generator_t {
    static_assert(!std::is_reference_v<ResultType>, "ResultType must be a reference");
    static_assert(!std::is_const_v<std::remove_reference_t<ResultType>>, "ResultType must not be const-qualified");
    static_assert(!std::is_volatile_v<std::remove_reference_t<ResultType>>,
                  "ResultType must not be volatile-qualified");

    class shared_state : public std::enable_shared_from_this<shared_state> {
        struct private_t {
            explicit private_t() = default;
        };

    public:
        shared_state(private_t)
            : _value(std::nullopt) {}

        static std::shared_ptr<shared_state> create() {
            return std::make_shared<shared_state>(private_t());
        }

        std::shared_ptr<shared_state> getptr() {
            return this->shared_from_this();
        }

        bool has_value() const noexcept {
            return _value.has_value();
        }

        void reset() noexcept {
            _value.reset();
        }

        template <typename SetValueArg>
        void set_value(SetValueArg&& arg) {
            reset();
            _value.emplace(std::forward<SetValueArg>(arg));
        }

        ResultType get_value() {
            auto result = std::move(_value).value();
            reset();
            return result;
        }

    private:
        std::optional<ResultType> _value {};
    };

public:
    struct yield_context {
        yield_context(suspend_context_i& suspend_context, std::weak_ptr<shared_state> state)
            : _suspend_context(suspend_context)
            , _state(std::move(state)) {}
        ~yield_context() = default;

        template <typename ValueType>
        void yield(ValueType&& value) {
            auto state = _state.lock();
            if (!state) {
                throw std::logic_error {"invalid state"};
            }

            state->set_value(std::forward<ValueType>(value));
            _suspend_context.suspend();
        }

    private:
        suspend_context_i& _suspend_context;
        std::weak_ptr<shared_state> _state;
    };
    using job_t = fu2::unique_function<void(yield_context&)>;

    generator_t(job_t in_job)
        : _state(shared_state::create())
        , _coroutine(coroutine_t::create([state = _state, job = std::move(in_job)](auto& self) mutable {
            std::weak_ptr<shared_state> weak_state(state);
            yield_context yield_context(self, std::move(weak_state));
            job(yield_context);
        })) {
        // TODO : fix _coroutine move
    }

    bool has_next() {
        fill();
        return _state->has_value();
    }

    ResultType next() {
        assert(_state->has_value());
        return _state->get_value();
    }

private:
    void fill() {
        assert(_coroutine.is_valid());
        if (_state->has_value()) {
            return;
        }

        if (_coroutine.is_done()) {
            return;
        }

        _coroutine.resume();
    }

private:
    // bool _is_full{};
    std::shared_ptr<shared_state> _state;
    coroutine_t _coroutine;
};

} // namespace walle::cortex
