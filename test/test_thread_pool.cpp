#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include <walle/core/thread_pool.hpp>

using namespace std::chrono_literals;

// Helper task to track execution count
void increment_counter(std::atomic<int>& counter) {
    std::this_thread::sleep_for(50ms); // Simulate work
    ++counter;
}

class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // This can be used to set up common preconditions for all tests
    }

    void TearDown() override {
        // This can be used to clean up after each test
    }
};

// --- Constructor Tests ---
TEST_F(ThreadPoolTest, Constructor_ValidWorkersCount) {
    walle::core::thread_pool pool(4);
    ASSERT_EQ(pool.workers_count(), 4); // Check worker thread count
    pool.stop();
}

TEST_F(ThreadPoolTest, Constructor_InvalidWorkersCount) {
    // This test assumes that an invalid worker count (0) might throw an exception or handle gracefully.
    EXPECT_THROW(walle::core::thread_pool pool(0), std::invalid_argument);
}

// --- Task Submission Tests ---
TEST_F(ThreadPoolTest, Submit_SingleTask) {
    walle::core::thread_pool pool(2);
    std::atomic<int> counter = 0;

    pool.submit([&counter] { increment_counter(counter); });
    pool.wait_idle();

    ASSERT_EQ(counter.load(), 1);
    pool.stop();
}

TEST_F(ThreadPoolTest, Submit_MultipleTasks) {
    walle::core::thread_pool pool(3);
    std::atomic<int> counter = 0;

    for (int i = 0; i < 123; ++i) {
        pool.submit([&counter] { increment_counter(counter); });
    }
    pool.wait_idle();

    ASSERT_EQ(counter.load(), 123); // All tasks should be completed
    pool.stop();
}

TEST_F(ThreadPoolTest, Submit_ConcurrentExecution) {
    walle::core::thread_pool pool(4);
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
    ASSERT_GT(thread_ids.size(), 1); // Multiple threads should have been used
    std::sort(thread_ids.begin(), thread_ids.end());
    thread_ids.erase(std::unique(thread_ids.begin(), thread_ids.end()), thread_ids.end());
    ASSERT_GT(thread_ids.size(), 1); // Check that multiple distinct threads ran the tasks
    pool.stop();
}

// --- Wait Idle Tests ---
TEST_F(ThreadPoolTest, WaitIdle_BlocksUntilTasksComplete) {
    walle::core::thread_pool pool(2);
    std::atomic<int> counter = 0;

    for (int i = 0; i < 5; ++i) {
        pool.submit([&counter] {
            std::this_thread::sleep_for(100ms); // Simulate slow task
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

// --- Thread Safety Tests ---
TEST_F(ThreadPoolTest, Submit_FromMultipleThreads) {
    walle::core::thread_pool pool(4);
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
    ASSERT_EQ(counter.load(), 15); // Each thread submitted 5 tasks
    pool.stop();
}
