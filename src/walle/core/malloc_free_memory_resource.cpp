#include "walle/core/malloc_free_memory_resource.hpp"

namespace walle::core {

malloc_free_memory_resource malloc_free_memory_resource::memory_resource {};

malloc_free_memory_resource* malloc_free_memory_resource::get_global() {
    return &memory_resource;
}

void* malloc_free_memory_resource::do_allocate(std::size_t bytes, std::size_t) {
    void* data = std::malloc(bytes);
    if (data == nullptr) {
        throw std::bad_alloc {};
    }
    return data;
}

void malloc_free_memory_resource::do_deallocate(void* p, std::size_t, std::size_t) {
    std::free(p);
}

bool malloc_free_memory_resource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    try {
        [[maybe_unused]] const auto& ref = dynamic_cast<const malloc_free_memory_resource&>(other);
    } catch (const std::bad_cast&) {
        return false;
    }
    return true;
}

} // namespace walle::core
