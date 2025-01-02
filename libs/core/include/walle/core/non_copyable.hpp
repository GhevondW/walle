#pragma once

namespace walle::core {

struct non_copyable {
    non_copyable() noexcept = default;
    ~non_copyable() noexcept = default;
    non_copyable(non_copyable const&) = delete;
    non_copyable& operator=(non_copyable const&) = delete;
    non_copyable(non_copyable&&) noexcept = default;
    non_copyable& operator=(non_copyable&&) noexcept = default;
}; // struct non_copyable

} // namespace walle::core
