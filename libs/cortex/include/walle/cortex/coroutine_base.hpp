#pragma once

#include <walle/cortex/coroutine.hpp>

namespace walle::cortex {

/**
 * @brief Abstract base class for managing coroutine flows.
 *
 * This class serves as an interface for coroutine implementations. It defines the
 * necessary methods to control the lifecycle of a coroutine, including its
 * creation, validation, and resumption. Derived classes must implement the
 * `flow` method to define the behavior of the coroutine.
 */
struct coroutine_base_i {
public:
    /**
     * @brief Default constructor for the coroutine base.
     */
    coroutine_base_i();

    /**
     * @brief Virtual destructor for the coroutine base class.
     */
    virtual ~coroutine_base_i() = default;

    /**
     * @brief Deleted copy constructor.
     *
     * Copying of coroutines is not allowed to ensure uniqueness.
     */
    coroutine_base_i(const coroutine_base_i&) = delete;

    /**
     * @brief Move constructor for the coroutine base class.
     *
     * Allows transferring ownership of the coroutine to a new instance.
     *
     * @param other The coroutine to move from.
     */
    coroutine_base_i(coroutine_base_i&&) noexcept;

    /**
     * @brief Deleted copy assignment operator.
     *
     * Copy assignment of coroutines is not allowed to ensure uniqueness.
     */
    coroutine_base_i& operator=(const coroutine_base_i&) = delete;

    /**
     * @brief Move assignment operator for the coroutine base class.
     *
     * Allows transferring ownership of the coroutine to a new instance.
     *
     * @param other The coroutine to move from.
     * @return A reference to the current instance.
     */
    coroutine_base_i& operator=(coroutine_base_i&&) noexcept;

    /**
     * @brief Checks if the coroutine is valid.
     *
     * @return `true` if the coroutine is valid, `false` otherwise.
     */
    explicit operator bool() const noexcept;

    /**
     * @brief Returns whether the coroutine is valid.
     *
     * @return `true` if the coroutine is valid, `false` otherwise.
     */
    bool is_valid() const noexcept;

    /**
     * @brief Resumes the coroutine.
     *
     * Resumes execution of the coroutine from the point it was suspended.
     *
     * @note Calling this function on an invalid coroutine causes undefined behavior
     *       and might throw a `resume_on_completed_coroutine_error_t`.
     */
    void resume();

    /**
     * @brief Checks if the coroutine has completed execution.
     *
     * @return `true` if the coroutine has finished, `false` otherwise.
     */
    [[nodiscard]] bool is_done() const noexcept;

private:
    /**
     * @brief Pure virtual function to define the coroutine's flow.
     *
     * Derived classes must implement this method to define the specific
     * behavior and execution steps of the coroutine.
     *
     * @param self The suspend context of the coroutine.
     */
    virtual void flow(suspend_context_i& self) = 0;

    void run_flow(suspend_context_i& self);

private:
    coroutine_t _coroutine; ///< The actual coroutine instance that manages the execution.
};

} // namespace walle::cortex
