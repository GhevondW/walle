#pragma once

/// @namespace walle::core
/// @brief Core namespace for fundamental utilities.
namespace walle::core {

/**
 * @struct non_copyable
 * @brief A utility struct to disable copy semantics for a class or struct.
 *
 * This struct prevents copy construction and copy assignment,
 * while allowing move construction and move assignment.
 */
struct non_copyable {
    non_copyable() noexcept = default;
    ~non_copyable() noexcept = default;
    non_copyable(non_copyable const&) = delete; ///< Deleted copy constructor.
    non_copyable& operator=(non_copyable const&) = delete; ///< Deleted copy assignment operator.
    non_copyable(non_copyable&&) noexcept = default; ///< Default move constructor.
    non_copyable& operator=(non_copyable&&) noexcept = default; ///< Default move assignment operator.
}; // struct non_copyable

} // namespace walle::core
