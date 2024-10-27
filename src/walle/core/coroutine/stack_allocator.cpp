#include "walle/core/coroutine/stack_allocator.hpp"
#include <cassert>

namespace walle::core::coroutine {

stack_allocator::stack_allocator(stack_allocator&& other) noexcept
    : _memory_resource(other._memory_resource)
    , _default_size(other._default_size) {
    other._memory_resource = nullptr;
}

stack_allocator& stack_allocator::operator=(stack_allocator&& other) noexcept {
    if (this != &other) {
        _memory_resource = other._memory_resource;
        _default_size = other._default_size;

        other._memory_resource = nullptr;
    }
    return *this;
}

[[nodiscard]] stack_allocator::stack_t stack_allocator::allocate() {
    assert(is_valid());

    void* data_ptr = _memory_resource->allocate(_default_size, alignof(std::max_align_t));
    if (data_ptr == nullptr) {
        throw std::bad_alloc();
    }

    return stack_t {static_cast<std::byte*>(data_ptr) + _default_size, _default_size};
}

void stack_allocator::deallocate(stack_t stack) const noexcept {
    assert(is_valid());
    assert(stack._ptr != nullptr);
    assert(stack._size = _default_size);

    void* ptr = stack.top();
    _memory_resource->deallocate(static_cast<std::byte*>(ptr) - _default_size, stack._size);
}

} // namespace walle::core::coroutine
