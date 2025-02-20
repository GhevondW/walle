#pragma once

namespace walle::asymtx {

// TODO make awaitable_traits for now this is ok
template <typename TaskType>
struct task_traits {
    using result_type_t = typename TaskType::result_type_t;
};

} // namespace walle::asymtx
