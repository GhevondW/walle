#include "walle/async/this_task.hpp"
#include "walle/async/error/error.hpp"

namespace walle::async {

void this_task::yield() {
    throw error::not_implemeted_error_t {"not implemented yet"};
}

task_id this_task::get_id() {
    throw error::not_implemeted_error_t {"not implemented yet"};
}

void this_task::sleep_for([[maybe_unused]] std::size_t milliseconds) {
    throw error::not_implemeted_error_t {"not implemented yet"};
}

} // namespace walle::async
