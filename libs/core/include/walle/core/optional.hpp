#pragma once

#include <cassert>
#include <utility>

/// @namespace walle::core
/// @brief Core namespace for fundamental utilities.
namespace walle::core {

/**
 * @brief Extracts the value of an optional-like object, asserting that it contains a value.
 *
 * This function asserts that the given optional-like object has a value before extracting it.
 * If the assertion fails, the program terminates.
 *
 * @tparam OptionalLike The type of the optional-like object (e.g., std::optional).
 * @param optional The optional-like object to extract the value from.
 * @return The extracted value.
 */
template <typename OptionalLike>
decltype(auto) asserted_value(OptionalLike&& optional) {
    assert(optional.has_value());
    return *(std::forward<OptionalLike>(optional));
}

} // namespace walle::core
