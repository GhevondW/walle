#pragma once

#include <memory>

#include <walle/core/atomic_single_shot_event.hpp>
#include <walle/core/non_copyable.hpp>

#include <walle/async/task_function.hpp>
#include <walle/async/task_id.hpp>

#include <boost/intrusive_ptr.hpp>
#include <walle/core/single_shot_event.hpp>

namespace walle::async {

class task_context;

// TODO : allocator support
class task_handle final : public core::non_copyable {
public:
    task_handle();
    task_handle(task_handle&&) noexcept;
    task_handle& operator=(task_handle&&) noexcept;
    ~task_handle();

    bool is_valid() const noexcept;

    bool joinable() const noexcept;

    task_id get_id() const noexcept;

    // void detach();

    // coroutine context
    void join();

    // none coroutine context
    void blocking_join();

    void cancel();

    void blocking_cancel();

private:
    task_context* _impl = nullptr;
    std::unique_ptr<core::atomic_single_shot_event_t> _event = nullptr;
};

} // namespace walle::async
