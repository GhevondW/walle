#include "walle/core/coroutine_stack_allocator.hpp"

#include <cassert>
#include <stdexcept>

namespace walle::core {

coroutine_stack_allocator::coroutine_stack_allocator(std::pmr::memory_resource* resource, std::size_t default_size)
    : _memory_resource(resource)
    , _default_size(default_size) {
    if (_default_size == 0) {
        throw std::logic_error {"the default size is zero"};
    }
}

coroutine_stack_allocator::coroutine_stack_allocator(coroutine_stack_allocator&& other) noexcept
    : _memory_resource(other._memory_resource)
    , _default_size(other._default_size) {
    other._memory_resource = nullptr;
}

coroutine_stack_allocator& coroutine_stack_allocator::operator=(coroutine_stack_allocator&& other) noexcept {
    if (this != &other) {
        _memory_resource = other._memory_resource;
        _default_size = other._default_size;

        other._memory_resource = nullptr;
    }
    return *this;
}

[[nodiscard]] coroutine_stack coroutine_stack_allocator::allocate() {
    assert(is_valid());

    void* data_ptr = _memory_resource->allocate(_default_size);

    return coroutine_stack {reinterpret_cast<std::byte*>(data_ptr) + _default_size, _default_size};
}

void coroutine_stack_allocator::deallocate(coroutine_stack stack) noexcept {
    assert(is_valid());
    assert(stack._ptr != nullptr);
    assert(stack._size == _default_size);

    void* ptr = stack.top();
    _memory_resource->deallocate(static_cast<std::byte*>(ptr) - _default_size, stack._size);
}

} // namespace walle::core
