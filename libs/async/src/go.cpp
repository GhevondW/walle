#include "walle/async/go.hpp"
#include "walle/async/error/error.hpp"

namespace walle::async {

task_handle go([[maybe_unused]] exec::executor_i* executor, [[maybe_unused]] task_function_t func) {
    throw error::not_implemeted_error_t {"not implemented yet."};
}

task_handle go([[maybe_unused]] task_function_t func) {
    throw error::not_implemeted_error_t {"not implemented yet."};
}

} // namespace walle::async
