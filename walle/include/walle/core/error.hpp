#pragma once

#include <stdexcept>

namespace walle::core {

class not_implemeted_error : public std::logic_error {
public:
    using logic_error::logic_error;
};

} // namespace walle::core
