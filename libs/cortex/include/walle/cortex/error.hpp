#pragma once

#include <stdexcept>

namespace walle::cortex {

/**
 * @brief Exception thrown when attempting to resume a completed coroutine.
 *
 * This exception is thrown when an operation attempts to resume a coroutine
 * that has already completed. It derives from `std::logic_error` to signal
 * a logical error in the coroutine's state management.
 */
struct resume_on_completed_coroutine_error_t : std::logic_error {
    /**
     * @brief Constructs a new `resume_on_completed_coroutine_error_t` exception.
     *
     * This constructor initializes the exception with an error message.
     *
     * @param msg The error message describing the issue.
     */
    using logic_error::logic_error;
};

} // namespace walle::cortex
