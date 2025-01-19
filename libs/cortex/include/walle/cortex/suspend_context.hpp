#pragma once

namespace walle::cortex {

/**
 * @brief Abstract base class for managing suspension context in coroutines.
 *
 * This class defines an interface for managing the suspension of coroutines. It is
 * used by coroutine-related components to suspend execution, allowing the coroutine
 * to be resumed later. The suspend functionality is essential for implementing cooperative
 * multitasking and managing state between coroutine yields.
 */
struct suspend_context_i {
    /**
     * @brief Default constructor for the suspend context interface.
     */
    suspend_context_i() = default;

    /**
     * @brief Deleted copy constructor.
     *
     * Copying of suspend context is not allowed to ensure uniqueness.
     */
    suspend_context_i(const suspend_context_i&) = delete;

    /**
     * @brief Deleted move constructor.
     *
     * Moving of suspend context is not allowed to ensure uniqueness.
     */
    suspend_context_i(suspend_context_i&&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * Copying of suspend context is not allowed to ensure uniqueness.
     */
    suspend_context_i& operator=(const suspend_context_i&) = delete;

    /**
     * @brief Deleted move assignment operator.
     *
     * Moving of suspend context is not allowed to ensure uniqueness.
     */
    suspend_context_i& operator=(suspend_context_i&&) = delete;

    /**
     * @brief Virtual destructor for the suspend context interface.
     */
    virtual ~suspend_context_i() = default;

    /**
     * @brief Pure virtual function to suspend execution of the current context.
     *
     * This method must be implemented by derived classes to suspend execution,
     * allowing the coroutine to be resumed later. The suspension operation is
     * platform-specific and may involve unwinding.
     *
     * @note This function cannot be `noexcept` because it may trigger forced unwinding.
     */
    virtual void suspend() = 0;
};

} // namespace walle::cortex
