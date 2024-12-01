#pragma once

#include <function2/function2.hpp>

namespace walle::core {

struct executor_i {
    using task_t = fu2::unique_function<void()>;

    executor_i() = default;
    executor_i(const executor_i& other) = delete;
    executor_i(executor_i&& other) noexcept = default;
    executor_i& operator=(const executor_i& other) = delete;
    executor_i& operator=(executor_i&& other) noexcept = delete;
    virtual ~executor_i() = default;

    virtual void submit(task_t task) = 0;
};

} // namespace walle::core
