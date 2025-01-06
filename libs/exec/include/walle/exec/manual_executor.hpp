#pragma once

#include <cstddef>
#include <queue>

#include <stdexcept>
#include <walle/core/non_copyable.hpp>
#include <walle/core/non_movable.hpp>
#include <walle/exec/executor.hpp>

namespace walle::exec {

// This class is used for testing.
// this is the only class that is not concurrent
class manual_executor
    : core::non_copyable
    , core::non_movable
    , public executor_i {
public:
    struct empty_executor : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    struct empty_task : std::logic_error {
        using std::logic_error::logic_error;
    };

    manual_executor() = default;
    ~manual_executor() override = default;

    bool submit(task_t task) override;

    void start() override;
    void stop() override;
    state_e state() const noexcept override {
        return _state;
    }

    [[nodiscard]] std::size_t tasks_count() const noexcept;

    // might throw empty_executor
    // strong exception guarantee
    void run_one();

    // throws nothing if the queue is empty does nothing.
    // base exception guarantee
    void run_all();

private:
    state_e _state {state_e::k_stopped};
    std::queue<task_t> _tasks {};
};

} // namespace walle::exec
