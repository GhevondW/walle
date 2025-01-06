#pragma once

#include <stdexcept>

#include <walle/core/non_copyable.hpp>
#include <walle/core/non_movable.hpp>
#include <walle/exec/executor.hpp>

namespace walle::exec {

struct current_executor {
    struct no_execution_context_error : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    struct scope_guard
        : private core::non_copyable
        , core::non_movable {
        explicit scope_guard(executor_i* current) noexcept;
        ~scope_guard();

    private:
        executor_i* _prev_executor = nullptr;
    };

    static executor_i* get();
    static executor_i* unsafe_get();

private:
    static void set(executor_i* current) noexcept;

    static thread_local executor_i* current_;
};

} // namespace walle::exec
