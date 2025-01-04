#include "walle/async/yield.hpp"
#include "walle/async/error/error.hpp"

namespace walle::async {

void yield() {
    throw error::not_implemeted_error_t {"not implemented yet."};
}

} // namespace walle::async
