#pragma once

#include <cstddef>

namespace walle::cortex {

struct stack_context {
    void* ptr = nullptr;
    std::size_t size = 0;
};

} // namespace walle::cortex
