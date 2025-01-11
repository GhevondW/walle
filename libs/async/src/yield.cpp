#include "walle/async/yield.hpp"
#include "fiber.hpp"
#include "walle/async/error/error.hpp"

namespace walle::async {

void yield() {
    auto& self_ref = fiber_t::self();
    self_ref.yield();
}

} // namespace walle::async
