#pragma once

#include <cstddef>
#include <walle/core/malloc_free_memory_resource.hpp>

namespace walle::core {

struct coroutine_stack {
    friend struct coroutine_stack_allocator;

    coroutine_stack() = default;

    [[nodiscard]] std::byte* top() const noexcept {
        return _ptr;
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return _size;
    }

    [[nodiscard]] bool is_valid() const noexcept {
        return _ptr != nullptr && _size != 0;
    }

private:
    coroutine_stack(void* ptr, std::size_t size) noexcept
        : _ptr(static_cast<std::byte*>(ptr))
        , _size(size) {}

    std::byte* _ptr {};
    std::size_t _size {};
};

struct coroutine_stack_allocator {
    explicit coroutine_stack_allocator(std::pmr::memory_resource* resource = malloc_free_memory_resource::get_global(),
                                       std::size_t default_size = 256000);

    coroutine_stack_allocator(const coroutine_stack_allocator& other) = default;
    coroutine_stack_allocator& operator=(const coroutine_stack_allocator& other) = delete;

    coroutine_stack_allocator(coroutine_stack_allocator&& other) noexcept;
    coroutine_stack_allocator& operator=(coroutine_stack_allocator&& other) noexcept;

    ~coroutine_stack_allocator() = default;

    [[nodiscard]] coroutine_stack allocate();
    void deallocate(coroutine_stack stack) noexcept;

    [[nodiscard]] bool is_valid() const noexcept {
        return _memory_resource != nullptr && _default_size != 0;
    }

private:
    std::pmr::memory_resource* _memory_resource {};
    std::size_t _default_size {};
};

} // namespace walle::core
