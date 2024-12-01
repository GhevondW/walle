#include "walle/core/utils/current_executor.hpp"

namespace walle::core::utils {

thread_local executor_i* current_executor::current_ = nullptr;

current_executor::scope_guard::scope_guard(executor_i* current) noexcept 
    :_prev_executor(current_)
{
    current_executor::set(current);
}

current_executor::scope_guard::~scope_guard() {
    current_executor::set(_prev_executor);
}

executor_i* current_executor::get() {
    if(current_ == nullptr) {
        throw no_execution_context_error{"there is no execution in this context"};
    }
    return current_;
}

void current_executor::set(executor_i* current) noexcept {
    assert(current != nullptr);
    current_ = current;
}

void current_executor::reset() noexcept {
    current_ = nullptr;
}

}