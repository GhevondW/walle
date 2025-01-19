#pragma once

/// @namespace walle::core::utils
/// @brief Utility namespace for additional helper constructs.
namespace walle::core::utils {

/**
 * @brief A helper type for creating overloaded function callables.
 *
 * This template enables combining multiple callable types (e.g., lambdas) into a single callable object.
 * Each callable's operator() is accessible through the combined object.
 *
 * @tparam Args The types of the callable objects to combine.
 *
 * ### Example Usage:
 * @code
 * #include <iostream>
 * #include "overloaded.hpp"
 *
 * using namespace walle::core::utils;
 *
 * int main() {
 *     auto visitor = overloaded_t{
 *         [](int i) { std::cout << "Integer: " << i << '\n'; },
 *         [](double d) { std::cout << "Double: " << d << '\n'; },
 *         [](const std::string& s) { std::cout << "String: " << s << '\n'; }
 *     };
 *
 *     visitor(42);         // Outputs: Integer: 42
 *     visitor(3.14);       // Outputs: Double: 3.14
 *     visitor("hello");   // Outputs: String: hello
 *     return 0;
 * }
 *
 * // Using with std::variant and std::visit
 * #include <variant>
 * #include <iostream>
 *
 * int main() {
 *     std::variant<int, double, std::string> v = 42;
 *
 *     // Create overloaded visitor to handle variant types
 *     auto visitor = overloaded_t{
 *         [](int i) { std::cout << "Integer: " << i << '\n'; },
 *         [](double d) { std::cout << "Double: " << d << '\n'; },
 *         [](const std::string& s) { std::cout << "String: " << s << '\n'; }
 *     };
 *
 *     // Visit the variant using the overloaded visitor
 *     std::visit(visitor, v);  // Outputs: Integer: 42
 *
 *     v = 3.14;
 *     std::visit(visitor, v);  // Outputs: Double: 3.14
 *
 *     v = "hello";
 *     std::visit(visitor, v);  // Outputs: String: hello
 *
 *     return 0;
 * }
 * @endcode
 */
template <typename... Args>
struct overloaded_t : Args... {
    using Args::operator()...; ///< Inherit all operator() methods from the callables.
};

/**
 * @brief Deduction guide for `overloaded_t`.
 *
 * This allows the compiler to deduce the template arguments for `overloaded_t` based on the arguments provided.
 *
 * @tparam Ts The types of the callable objects to combine.
 */
template <class... Ts>
overloaded_t(Ts...) -> overloaded_t<Ts...>;

}; // namespace walle::core::utils
