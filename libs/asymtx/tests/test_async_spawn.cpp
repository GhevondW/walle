#include <gtest/gtest.h>
#include <walle/asymtx/async_spawn.hpp>
#include <walle/asymtx/mutex.hpp>
#include <walle/asymtx/sync_spawn.hpp>
#include <walle/core/atomic_single_shot_event.hpp>
#include <walle/exec/event_loop.hpp>
#include <walle/exec/manual_executor.hpp>
#include <walle/exec/thread_pool.hpp>

walle::asymtx::mutex_t global_counter_mutex;
std::size_t global_counter = 0; // Guarded by global_counter_mutex

TEST(asymtx_sync_task, just_works_async_spawn) {
    using namespace walle;

    global_counter = 0;

    exec::thread_pool pool(1);
    asymtx::scheduler_t sched(pool);

    std::cout << "Main thread started : " << std::this_thread::get_id() << std::endl;

    auto main_task_handle = asymtx::sync_spawn(sched, [](asymtx::scheduler_t& sched) -> walle::asymtx::task_t<> {
        std::cout << "Main task started : " << std::this_thread::get_id() << std::endl;

        auto inner1 = asymtx::async_scope();
        auto inner2 = asymtx::async_scope();
        auto inner3 = asymtx::async_scope();

        inner2.spawn([&sched]() -> asymtx::task_t<> {
            co_await sched.schedule();
            std::cout << "Inner task started 2: " << std::this_thread::get_id() << std::endl;

            co_await global_counter_mutex.lock();
            ++global_counter;
            global_counter_mutex.unlock();

            co_return;
        }());

        inner3.spawn([&sched]() -> asymtx::task_t<> {
            co_await sched.schedule();
            std::cout << "Inner task started 3: " << std::this_thread::get_id() << std::endl;

            co_await global_counter_mutex.lock();
            ++global_counter;
            global_counter_mutex.unlock();

            co_return;
        }());

        inner1.spawn([&sched]() -> asymtx::task_t<> {
            co_await sched.schedule();
            std::cout << "Inner task started 1: " << std::this_thread::get_id() << std::endl;

            co_await global_counter_mutex.lock();
            ++global_counter;
            global_counter_mutex.unlock();

            co_return;
        }());

        co_await inner1.join();
        co_await inner2.join();
        co_await inner3.join();

        co_return;
    }(sched));

    main_task_handle.blocking_join();
    pool.stop();

    EXPECT_EQ(global_counter, 3);
}