#include "walle/exec/current_executor.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

#include <walle/exec/thread_pool.hpp>

using namespace std::chrono_literals;

void increment_counter(std::atomic<int>& counter) {
    std::this_thread::sleep_for(50ms);
    ++counter;
}

class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(ThreadPoolTest, constructor_valid_workers_count) {
    walle::exec::thread_pool pool(4);
    ASSERT_EQ(pool.workers_count(), 4);
    pool.stop();
}

TEST_F(ThreadPoolTest, constructor_invalid_workers_count) {
    EXPECT_THROW(walle::exec::thread_pool pool(0), std::invalid_argument);
}

TEST_F(ThreadPoolTest, submit_single_task) {
    walle::exec::thread_pool pool(2);
    std::atomic<int> counter = 0;

    pool.submit([&counter] { increment_counter(counter); });
    pool.wait_idle();

    ASSERT_EQ(counter.load(), 1);
    pool.stop();
}

TEST_F(ThreadPoolTest, submit_task_current_executor) {
    walle::exec::thread_pool pool(2);
    std::atomic<int> counter = 0;

    pool.submit([&counter] {
        increment_counter(counter);
        ASSERT_TRUE(walle::exec::current_executor::get() != nullptr);
        walle::exec::current_executor::get()->submit([&counter]() {
            increment_counter(counter);
            walle::exec::current_executor::get()->submit([&counter]() { increment_counter(counter); });
        });
    });
    pool.wait_idle();

    ASSERT_EQ(counter.load(), 3);
    pool.stop();
}

TEST_F(ThreadPoolTest, submit_multiple_tasks) {
    walle::exec::thread_pool pool(3);
    std::atomic<int> counter = 0;

    for (int i = 0; i < 123; ++i) {
        pool.submit([&counter] { increment_counter(counter); });
    }
    pool.wait_idle();

    ASSERT_EQ(counter.load(), 123);
    pool.stop();
}

TEST_F(ThreadPoolTest, submit_concurrent_execution) {
    walle::exec::thread_pool pool(4);
    std::atomic<int> counter = 0;
    std::mutex mtx;
    std::vector<std::thread::id> thread_ids;

    for (int i = 0; i < 10; ++i) {
        pool.submit([&counter, &mtx, &thread_ids] {
            {
                std::lock_guard<std::mutex> lock(mtx);
                thread_ids.push_back(std::this_thread::get_id());
            }
            increment_counter(counter);
        });
    }
    pool.wait_idle();

    ASSERT_EQ(counter.load(), 10);
    ASSERT_GT(thread_ids.size(), 1);
    std::sort(thread_ids.begin(), thread_ids.end());
    thread_ids.erase(std::unique(thread_ids.begin(), thread_ids.end()), thread_ids.end());
    ASSERT_GT(thread_ids.size(), 1);
    pool.stop();
}

TEST_F(ThreadPoolTest, wait_idle_blocks_until_tasks_complete) {
    walle::exec::thread_pool pool(2);
    std::atomic<int> counter = 0;

    for (int i = 0; i < 5; ++i) {
        pool.submit([&counter] {
            std::this_thread::sleep_for(100ms);
            increment_counter(counter);
        });
    }

    auto start_time = std::chrono::steady_clock::now();
    pool.wait_idle();
    auto end_time = std::chrono::steady_clock::now();

    ASSERT_EQ(counter.load(), 5);
    ASSERT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count(), 100);
    pool.stop();
}

// // --- Stop Tests ---
// TEST_F(ThreadPoolTest, Stop_PreventsFurtherTaskExecution) {
//     walle::core::thread_pool pool(2);
//     std::atomic<int> counter = 0;

//     pool.submit([&counter] { increment_counter(counter); });
//     pool.stop();

//     // Submitting a new task after stop() should have no effect
//     pool.submit([&counter] { increment_counter(counter); });
//     // TODO we can not use wait_idle if the pool is stopped.
//     pool.wait_idle();

//     ASSERT_EQ(counter.load(), 1);
// }

TEST_F(ThreadPoolTest, submit_from_multiple_threads) {
    walle::exec::thread_pool pool(4);
    std::atomic<int> counter = 0;

    auto submit_task = [&pool, &counter] {
        for (int i = 0; i < 5; ++i) {
            pool.submit([&counter] { increment_counter(counter); });
        }
    };

    std::thread t1(submit_task);
    std::thread t2(submit_task);
    std::thread t3(submit_task);
    t1.join();
    t2.join();
    t3.join();

    pool.wait_idle();
    ASSERT_EQ(counter.load(), 15);
    pool.stop();
}
