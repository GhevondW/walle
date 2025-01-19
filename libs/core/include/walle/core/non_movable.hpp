#pragma once

/// @namespace walle::core
/// @brief Core namespace for fundamental utilities.
namespace walle::core {

/**
 * @struct non_movable
 * @brief A utility struct to disable move semantics for a class or struct.
 *
 * This struct prevents move construction and move assignment,
 * while allowing copy construction and copy assignment.
 */
struct non_movable {
    non_movable() noexcept = default;
    ~non_movable() noexcept = default;
    non_movable(non_movable&&) = delete; ///< Deleted move constructor.
    non_movable& operator=(non_movable&&) = delete; ///< Deleted move assignment operator.
    non_movable(non_movable const&) = default; ///< Default copy constructor.
    non_movable& operator=(non_movable const&) = default; ///< Default copy assignment operator.
}; // struct non_movable

} // namespace walle::core
