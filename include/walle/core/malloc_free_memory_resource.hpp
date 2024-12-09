#pragma once

#include <cassert>
#include <cstdlib>
#include <memory_resource>
#include <new>
#include <typeinfo>

namespace walle::core {

class malloc_free_memory_resource : public std::pmr::memory_resource {
public:
    static malloc_free_memory_resource* get_global();

    malloc_free_memory_resource() = default;
    ~malloc_free_memory_resource() override = default;

    // Might throw an exception
    void* do_allocate(std::size_t bytes, std::size_t) override;
    void do_deallocate(void* p, std::size_t, std::size_t) override;
    [[nodiscard]] bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

private:
    static malloc_free_memory_resource memory_resource;
};

} // namespace walle::core
