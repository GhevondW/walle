#pragma once

#include <function2/function2.hpp>
#include <memory>
#include <walle/cortex/suspend_context.hpp>

namespace walle::cortex {

/**
 * @brief Represents a coroutine in the system.
 *
 * This class provides a wrapper for a coroutine that supports creation, resumption,
 * and state management. It uses `unique_function` to define the flow of the coroutine,
 * which can be executed when resumed. The `coroutine_t` class ensures proper lifetime
 * management and provides mechanisms to check if the coroutine has completed.
 */
class coroutine_t final {
    struct impl;

    explicit coroutine_t(std::shared_ptr<impl> in_impl) noexcept;

public:
    /**
     * @brief Alias for the coroutine flow type.
     *
     * The flow represents the coroutine's main execution function that gets invoked
     * when the coroutine is resumed. It accepts a `suspend_context_i` reference,
     * which allows suspension and resumption of the coroutine.
     */
    using flow_t = fu2::unique_function<void(suspend_context_i&)>;

    /**
     * @brief Creates a coroutine with an empty flow.
     *
     * This static function creates a coroutine object without a predefined flow, allowing
     * it to be initialized later.
     *
     * @return A new coroutine object.
     */
    static coroutine_t create();

    /**
     * @brief Creates a coroutine with the provided flow.
     *
     * This static function creates a coroutine object with the given flow, which will
     * define the coroutine's execution when resumed.
     *
     * @param in_flow The flow function that will be executed when the coroutine is resumed.
     * @return A new coroutine object.
     */
    static coroutine_t create(flow_t in_flow);

    /**
     * @brief Destructor.
     *
     * Destroys the coroutine object, ensuring that any resources associated with the coroutine
     * are properly cleaned up.
     */
    ~coroutine_t();

    /**
     * @brief Deleted copy constructor.
     *
     * The coroutine cannot be copied due to its internal state management.
     */
    coroutine_t(const coroutine_t&) = delete;

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of the coroutine object to a new instance.
     *
     * @param other The coroutine to be moved.
     */
    coroutine_t(coroutine_t&&) noexcept;

    /**
     * @brief Deleted copy assignment operator.
     *
     * The coroutine cannot be copied by assignment due to its internal state management.
     */
    coroutine_t& operator=(const coroutine_t&) = delete;

    /**
     * @brief Move assignment operator.
     *
     * Transfers ownership of the coroutine object to the target instance.
     *
     * @param other The coroutine to be moved.
     * @return A reference to the assigned coroutine.
     */
    coroutine_t& operator=(coroutine_t&&) noexcept;

    /**
     * @brief Implicit conversion to boolean.
     *
     * This operator allows the coroutine to be implicitly checked for validity.
     * It returns `true` if the coroutine is valid and `false` otherwise.
     *
     * @return `true` if the coroutine is valid, `false` otherwise.
     */
    explicit operator bool() const noexcept;

    /**
     * @brief Checks if the coroutine is valid.
     *
     * This function checks if the coroutine object has been properly initialized and
     * is in a valid state. If the coroutine is not valid, further operations like
     * `resume()` will result in undefined behavior.
     *
     * @return `true` if the coroutine is valid, `false` otherwise.
     */
    bool is_valid() const noexcept;

    /**
     * @brief Resumes the coroutine.
     *
     * This function resumes the coroutine from where it was last suspended. If the
     * coroutine is invalid, it may cause undefined behavior (UB).
     * It might also throw a `resume_on_completed_coroutine_error_t` exception if the
     * coroutine is already completed.
     */
    void resume();

    /**
     * @brief Checks if the coroutine has completed execution.
     *
     * This function checks whether the coroutine has finished execution. If the coroutine
     * is not done, it can still be resumed.
     *
     * @return `true` if the coroutine has completed, `false` otherwise.
     */
    [[nodiscard]] bool is_done() const noexcept;

private:
    std::shared_ptr<impl> _impl;
};

} // namespace walle::cortex
