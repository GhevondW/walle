#pragma once

#include <stdexcept>

namespace walle::async::error {

class not_implemeted_error_t : public std::logic_error {
public:
    using std::logic_error::logic_error;
};

} // namespace walle::async::error
