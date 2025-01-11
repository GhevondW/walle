#pragma once

#include "fiber_context.hpp"

#include <walle/cortex/coroutine.hpp>
#include <walle/exec/executor.hpp>

#include <function2/function2.hpp>

namespace walle::async {

// TODO : allocator support
class fiber_t : public fiber_context_i {
public:
    using flow_t = fu2::unique_function<void()>;

    static fiber_t* make_fiber(exec::executor_i* executor, flow_t flow);
    ~fiber_t() = default;

    void yield() override;
    void suspend() override;
    void resume() override;

    exec::executor_i* scheduler() const override;

    void schedule();

    static fiber_context_i& self();

private:
    fiber_t(exec::executor_i* executor, flow_t flow);
    void run();
    void destroy();

private:
    exec::executor_i* _executor {nullptr};
    cortex::coroutine::suspend_context* _suspend_context {nullptr};
    flow_t _flow;
    cortex::coroutine _coroutine {};
};

} // namespace walle::async
