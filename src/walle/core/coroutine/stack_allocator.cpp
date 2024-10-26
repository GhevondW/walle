#include "walle/core/coroutine/stack_allocator.hpp"

namespace walle::core::coroutine {

[[nodiscard]] stack_allocator::stack_t stack_allocator::allocate() {
    return stack_t {_memory_resource->allocate(_default_size), _default_size};
}

void stack_allocator::deallocate(stack_t stack) noexcept {
    _memory_resource->deallocate(stack._ptr, stack._size);
}

} // namespace walle::core::coroutine
