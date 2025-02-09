#pragma once

#include <function2/function2.hpp>

namespace walle::exec {

struct executor_i {
    using task_t = fu2::unique_function<void()>;

    executor_i() = default;
    virtual ~executor_i() = default;

    virtual void submit(task_t task) = 0;
    // maybe i need this
    // virtual void stop() = 0;
};

} // namespace walle::exec
