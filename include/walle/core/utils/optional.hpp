#pragma once

#include <cassert>
#include <utility>

namespace walle::core::utils {

template <typename OptionalLike>
decltype(auto) asserted_value(OptionalLike&& optional) {
    assert(optional.has_value());
    return *(std::forward<OptionalLike>(optional));
}

} // namespace walle::core::utils
