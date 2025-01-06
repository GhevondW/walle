#pragma once

#include <function2/function2.hpp>

namespace walle::exec {

struct executor_i {
    using task_t = fu2::unique_function<void()>;

    enum class state_e {
        k_stopped,
        k_running
    };

    executor_i() = default;
    virtual ~executor_i() = default;

    // onley return false when the executor is not started
    virtual bool submit(task_t task) = 0;

    // start has no effect if the executor is already started
    // the users must start and stop one time the executor
    virtual void start() = 0;
    // stop has no effect if the executor is already stopped.
    virtual void stop() = 0;

    // TODO remove state.
    virtual state_e state() const noexcept = 0;
};

} // namespace walle::exec
