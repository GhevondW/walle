#include <gtest/gtest.h>
#include <walle/asymtx/mutex.hpp>
#include <walle/asymtx/sync_spawn.hpp>
#include <walle/core/atomic_single_shot_event.hpp>
#include <walle/exec/thread_pool.hpp>

namespace {

std::atomic<std::size_t> counter = 0;

walle::asymtx::task_t<> foo() {
    std::cout << "Hello foo" << std::endl;
    ++counter;
    co_return;
}

walle::asymtx::task_t<> bar() {
    std::cout << "Hello bar" << std::endl;
    ++counter;
    co_return;
}

} // namespace

walle::asymtx::mutex_t global_counter_mutex;
std::size_t global_counter = 0; // Guarded by global_counter_mutex

TEST(asymtx_sync_task, just_works_spawn) {
    using namespace walle;

    global_counter = 0;

    exec::thread_pool pool(4);
    asymtx::scheduler_t sched(pool);

    std::cout << "Main thread started : " << std::this_thread::get_id() << std::endl;

    auto main_task_handle = asymtx::sync_spawn(sched, [](asymtx::scheduler_t& sched) -> walle::asymtx::task_t<> {
        std::cout << "Main task started : " << std::this_thread::get_id() << std::endl;
        co_await foo();

        auto inner_one = asymtx::sync_spawn(sched, []() -> asymtx::task_t<> {
            std::cout << "Inner one task started : " << std::this_thread::get_id() << std::endl;
            co_await foo();
            co_await foo();

            {
                co_await global_counter_mutex.lock();
                ++global_counter;
                global_counter_mutex.unlock();
            }

            co_return;
        }());

        auto inner_two = asymtx::sync_spawn(sched, []() -> asymtx::task_t<> {
            std::cout << "Inner two task started : " << std::this_thread::get_id() << std::endl;
            co_await bar();
            co_await bar();

            {
                co_await global_counter_mutex.lock();
                ++global_counter;
                global_counter_mutex.unlock();
            }

            co_return;
        }());

        {
            co_await global_counter_mutex.lock();
            ++global_counter;
            global_counter_mutex.unlock();
        }

        inner_two.blocking_join();
        inner_one.blocking_join();
        co_return;
    }(sched));

    main_task_handle.blocking_join();
    pool.stop();

    EXPECT_EQ(global_counter, 3);
}