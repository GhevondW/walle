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

/**
 * @brief A generator class that produces values lazily, suspending and resuming execution.
 *
 * This class provides a mechanism for generating a sequence of values lazily. It uses
 * coroutines and a `yield_context` to suspend execution and yield values. The `generator_t`
 * can be used to iterate over a sequence of values without generating them all upfront,
 * reducing memory usage.
 *
 * @tparam ResultType The type of values yielded by the generator.
 *
 * ### Example usage
 * @code
 * // Example usage of generator_t to generate values lazily
 * generator_t<int>::job_t job = [](yield_context& yield) {
 *     for (int i = 0; i < 5; ++i) {
 *         yield.yield(i);  // Yielding values from 0 to 4
 *     }
 * };
 *
 * generator_t<int> gen(job);
 * while (gen.has_next()) {
 *     int value = gen.next();
 *     std::cout << value << std::endl;  // Output: 0, 1, 2, 3, 4
 * }
 * @endcode
 */
template <typename ResultType>
class generator_t final {
    static_assert(!std::is_reference_v<ResultType>, "ResultType must not be a reference");
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

        template <typename... SetValueArgs>
        void set_value(SetValueArgs&&... args) {
            reset();
            _value.emplace(std::forward<SetValueArgs>(args)...);
        }

        ResultType get_value() {
            auto result = std::move(_value).value();
            reset();
            return result;
        }

    private:
        std::optional<ResultType> _value {};
    };

    struct yield_context {
        yield_context(suspend_context_i& suspend_context, std::weak_ptr<shared_state> state)
            : _suspend_context(suspend_context)
            , _state(std::move(state)) {}
        yield_context(const yield_context&) = delete;
        yield_context(yield_context&&) = delete;
        yield_context& operator=(const yield_context&) = delete;
        yield_context& operator=(yield_context&&) = delete;
        ~yield_context() = default;

        template <typename... Args>
        void yield(Args&&... args) {
            auto state = _state.lock();
            if (!state) {
                throw std::logic_error {"invalid state"};
            }

            state->set_value(std::forward<Args>(args)...);
            _suspend_context.suspend();
        }

    private:
        suspend_context_i& _suspend_context;
        std::weak_ptr<shared_state> _state;
    };

public:
    /**
     * @brief Type alias for the job function that defines the generator's behavior.
     *
     * The `job_t` type is a unique function that accepts a `yield_context` reference.
     * The job function defines the generator's flow and produces values when `yield` is called.
     */
    using job_t = fu2::unique_function<void(yield_context&)>;

    /**
     * @brief Constructs a generator with the specified job function.
     *
     * This constructor creates a generator by taking a job function that defines the flow
     * of the generator. The job function is invoked when the generator is resumed.
     *
     * @param in_job The job function that defines the generator's flow and produces values.
     */
    generator_t(job_t in_job);

    /**
     * @brief Checks if the generator has more values to yield.
     *
     * This function checks whether the generator has more values to yield by calling
     * the internal `fill` function and verifying if the generator's state has a value.
     *
     * @return `true` if the generator has more values to yield, `false` otherwise.
     */
    bool has_next();

    /**
     * @brief Gets the next value from the generator.
     *
     * This function retrieves the next value produced by the generator. It asserts that
     * the generator has a value available and returns the value, resetting the generator's state.
     *
     * @return The next value produced by the generator.
     *
     * @throws std::logic_error If the generator state is invalid or the value is not available.
     */
    ResultType next();

private:
    /**
     * @brief Fills the generator with the next value.
     *
     * This function resumes the coroutine if the generator is not done and does not
     * have a value. It ensures that the generator is filled with the next value before
     * it can be accessed.
     */
    void fill();

private:
    std::shared_ptr<shared_state> _state; ///< The shared state of the generator.
    coroutine_t _coroutine; ///< The coroutine responsible for executing the generator's flow.
};

template <typename ResultType>
generator_t<ResultType>::generator_t(job_t in_job)
    : _state(shared_state::create())
    , _coroutine(coroutine_t::create()) {
    auto wstate = std::weak_ptr<shared_state>(_state);
    _coroutine = coroutine_t::create([weak_state = std::move(wstate), job = std::move(in_job)](auto& self) mutable {
        yield_context yield_context(self, std::move(weak_state));
        job(yield_context);
    });
}

template <typename ResultType>
bool generator_t<ResultType>::has_next() {
    fill();
    return _state->has_value();
}

template <typename ResultType>
ResultType generator_t<ResultType>::next() {
    assert(_state->has_value());
    return _state->get_value();
}

template <typename ResultType>
void generator_t<ResultType>::fill() {
    assert(_coroutine.is_valid());
    if (_state->has_value()) {
        return;
    }

    if (_coroutine.is_done()) {
        return;
    }

    _coroutine.resume();
}

} // namespace walle::cortex
