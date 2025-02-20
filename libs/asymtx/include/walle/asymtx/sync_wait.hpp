#pragma once

#include <type_traits>
#include <walle/asymtx/sync_task.hpp>
#include <walle/asymtx/task.hpp>

namespace walle::asymtx {

template <typename ResultType>
ResultType sync_wait(task_t<ResultType>&& task) {
    core::atomic_single_shot_event_t event;
    auto sync_task = [](task_t<ResultType> task) -> sync_task_t<ResultType> {
        if constexpr (std::is_same_v<ResultType, void>) {
            co_await std::move(task);
        } else {
            // Temporary solution
            auto res = co_await std::move(task);
            co_return std::move(res);
        }
    }(std::move(task));

    sync_task.start(&event);
    event.wait();

    return std::move(sync_task).detach();
}

} // namespace walle::asymtx
