#pragma once

#include <function2/function2.hpp>

namespace walle::async {

using fiber_task_t = fu2::unique_function<void()>;

}
