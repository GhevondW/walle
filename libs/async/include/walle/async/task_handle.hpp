#pragma once

#include <memory>

#include <walle/core/atomic_single_shot_event.hpp>
#include <walle/core/non_copyable.hpp>

#include <walle/async/task_id.hpp>

// #include <boost/intrusive_ptr.hpp>
#include <walle/core/single_shot_event.hpp>

namespace walle::async {

class task_context_t;

// TODO : allocator support
class task_handle_t final : public core::non_copyable {
public:
    task_handle_t(std::shared_ptr<core::atomic_single_shot_event_t> event, task_context_t* task_context);
    task_handle_t(task_handle_t&&) noexcept;
    task_handle_t& operator=(task_handle_t&&) noexcept;
    ~task_handle_t();

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
    std::shared_ptr<core::atomic_single_shot_event_t> _event = nullptr;
    task_context_t* _impl = nullptr;
};

} // namespace walle::async
