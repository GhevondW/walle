#pragma once

namespace walle::core::utils {

template <typename... Args>
struct overloaded_t : Args... {
    using Args::operator()...;
};

template <class... Ts>
overloaded_t(Ts...) -> overloaded_t<Ts...>;

}; // namespace walle::core::utils
