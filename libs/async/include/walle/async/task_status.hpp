#pragma once

namespace walle::async {

enum class task_status_e {
    k_invalid = 0,
    k_new,
    k_running,
    k_suspended,
    k_completed
};

} // namespace walle::async
