#pragma once

#include "walle/async/fiber_task.hpp"

#include <walle/exec/executor.hpp>

namespace walle::async {

struct session_i {
    session_i() noexcept = default;
    virtual ~session_i() = default;

    void run(fiber_task_t main_task);

    virtual exec::executor_i* get_executor() const = 0;

    virtual void start() = 0;
    virtual void stop() = 0;
};

struct this_session {
    static session_i* active();
};

} // namespace walle::async
