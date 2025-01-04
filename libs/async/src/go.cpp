#include "walle/async/go.hpp"
#include "walle/async/error/error.hpp"

namespace walle::async {

void go([[maybe_unused]] exec::executor_i& executor, [[maybe_unused]] fiber_task_t task) {
    throw error::not_implemeted_error_t {"not implemented yet."};
}

void go([[maybe_unused]] fiber_task_t task) {
    throw error::not_implemeted_error_t {"not implemented yet."};
}

} // namespace walle::async
