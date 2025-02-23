#pragma once

#include <stdexcept>

#include <walle/core/non_copyable.hpp>
#include <walle/core/non_movable.hpp>
#include <walle/exec/executor.hpp>

namespace walle::exec {

/**
 * @brief Manages the currently active executor in a thread-local scope.
 *
 * `current_executor` allows access to the executor associated with the current execution context.
 */
struct current_executor {
    /**
     * @brief Exception thrown when attempting to access an executor in a non-execution context.
     */
    struct no_execution_context_error : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    /**
     * @brief RAII-style scope guard for temporarily setting the current executor.
     *
     * Ensures that the previous executor is restored when the guard goes out of scope.
     */
    struct scope_guard
        : private core::non_copyable
        , core::non_movable {
        /**
         * @brief Sets the current executor for the duration of the scope.
         * @param current The executor to set as the current executor.
         */
        explicit scope_guard(executor_i* current) noexcept;

        /**
         * @brief Restores the previous executor when the guard is destroyed.
         */
        ~scope_guard();

    private:
        executor_i* _prev_executor = nullptr; ///< Stores the previous executor.
    };

    /**
     * @brief Retrieves the executor associated with the current execution context.
     * @return Pointer to the current executor.
     * @throws no_execution_context_error if no executor is set.
     */
    static executor_i* get();

private:
    /**
     * @brief Sets the executor for the current execution context.
     * @param current The executor to set.
     */
    static void set(executor_i* current) noexcept;

    static thread_local executor_i* current_; ///< Thread-local storage for the current executor.
};

} // namespace walle::exec
