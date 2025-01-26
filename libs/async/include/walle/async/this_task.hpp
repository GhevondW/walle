#pragma once

#include <walle/async/task_id.hpp>
#include <walle/core/non_copyable.hpp>
#include <walle/core/non_movable.hpp>

namespace walle::async {

class task_context;

class this_task {
    friend class task_context;

public:
    static void yield();
    static task_id get_id();
    static void sleep_for(std::size_t milliseconds);

private:
    // TODO : move this in src
    struct guard
        : core::non_movable
        , core::non_copyable {
        guard(task_context* ctx);
        ~guard();
    };
    static void set(task_context* ctx) noexcept;
};

} // namespace walle::async
