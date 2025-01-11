#pragma once

#include <function2/function2.hpp>

namespace walle::exec {

// Maybe scheduler? :)
struct executor_i {
    using task_t = fu2::unique_function<void()>;

    executor_i() = default;
    virtual ~executor_i() = default;

    virtual void submit(task_t task) = 0;
};

} // namespace walle::exec
