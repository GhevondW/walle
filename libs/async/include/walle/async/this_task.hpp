#pragma once

#include <walle/async/task_id.hpp>

namespace walle::async {

class this_task {
public:
    static void yield();
    static task_id get_id();
    static void sleep_for(std::size_t milliseconds);
};

} // namespace walle::async
