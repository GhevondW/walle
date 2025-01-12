#pragma once

#include <stdexcept>

namespace walle::cortex {

struct resume_on_completed_coroutine_error_t : std::logic_error {
    using logic_error::logic_error;
};

} // namespace walle::cortex
