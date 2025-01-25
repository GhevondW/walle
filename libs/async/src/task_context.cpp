#include "task_context.hpp"
#include <cassert>

namespace walle::async {

void intrusive_ptr_add_ref(task_context* context) noexcept {
    assert(context);
    context->_use_count.fetch_add(1);
}

void intrusive_ptr_release(task_context* context) noexcept {
    assert(context);
    context->_use_count.fetch_sub(1);
}

} // namespace walle::async
