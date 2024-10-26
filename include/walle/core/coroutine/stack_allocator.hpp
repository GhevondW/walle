#pragma once

#include <cstddef>
#include <memory_resource>

namespace walle::core::coroutine {

struct stack_allocator {
    struct stack_t {
        friend struct stack_allocator;

        [[nodiscard]] std::byte* top() const noexcept {
            return _ptr;
        }

        [[nodiscard]] std::size_t size() const noexcept {
            return _size;
        }

    private:
        stack_t(void* ptr, std::size_t size) noexcept
            : _ptr(static_cast<std::byte*>(ptr))
            , _size(size) {}

        std::byte* _ptr {};
        std::size_t _size {};
    };

    explicit stack_allocator(std::pmr::memory_resource* resource = std::pmr::get_default_resource(),
                             std::size_t default_size = 250000)
        : _memory_resource(resource)
        , _default_size(default_size) {}

    [[nodiscard]] stack_t allocate();
    void deallocate(stack_t stack) noexcept;

private:
    std::pmr::memory_resource* _memory_resource {};
    std::size_t _default_size {};
};

} // namespace walle::core::coroutine
