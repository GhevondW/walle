#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <walle/core/wait_group.hpp>
#include <walle/exec/current_executor.hpp>
#include <walle/exec/event_loop.hpp>
#include <walle/exec/executor.hpp> // Replace with actual path to executor_i and event_loop headers

using namespace walle::exec;

// Helper function to simulate a task
void dummy_task(std::atomic<int>& counter) {
    ++counter;
}

class EventLoopTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Any setup code (if needed)
    }

    void TearDown() override {
        // Any cleanup code (if needed)
    }
};

// Test that the constructor initializes correctly
TEST_F(EventLoopTest, ConstructorWorks) {
    auto loop = event_loop::make();
    // Verify the event loop initializes without issues
    loop->stop();
    SUCCEED(); // If no crash happens, the test passes
}

// Test submitting a task and ensuring it executes
TEST_F(EventLoopTest, SubmitsTaskAndExecutes) {
    auto loop = event_loop::make();
    std::atomic<int> counter = 0;

    loop->submit([&]() { dummy_task(counter); });

    // Wait for the task to execute
    loop->stop(); // TODO use wait_group
    ASSERT_EQ(counter, 1);
}

// Test multiple task submissions
TEST_F(EventLoopTest, ExecutesMultipleTasks) {
    auto loop = event_loop::make();
    std::atomic<int> counter = 0;

    for (int i = 0; i < 5; ++i) {
        loop->submit([&]() { dummy_task(counter); });
    }

    // Wait for tasks to execute
    loop->stop();
    ASSERT_EQ(counter, 5);
}

// Test stopping the event loop
TEST_F(EventLoopTest, StopsEventLoop) {
    auto loop = event_loop::make();
    std::atomic<int> counter = 0;

    loop->submit([&]() { dummy_task(counter); });
    loop->stop();

    // Submit another task after stopping
    loop->submit([&]() { dummy_task(counter); });

    // Wait to ensure tasks are not executed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(counter, 1); // Only the first task should execute
}

// Test destructor cleans up properly
TEST_F(EventLoopTest, DestructorStopsAndCleansUp) {
    std::atomic<int> counter = 0;

    auto loop = event_loop::make();
    loop->submit([&]() { dummy_task(counter); });

    loop->stop();
    // Give some time for destructor cleanup
    ASSERT_EQ(counter, 1); // Task should have executed before destruction
}

// Test edge case: no tasks submitted
TEST_F(EventLoopTest, NoTasksSubmitted) {
    auto loop = event_loop::make();
    loop->stop();
    SUCCEED(); // Ensure no crash occurs
}

// Test submitting tasks after the loop is stopped
TEST_F(EventLoopTest, SubmitAfterStopDoesNothing) {
    auto loop = event_loop::make();
    loop->stop();

    std::atomic<int> counter = 0;
    loop->submit([&]() { dummy_task(counter); });

    // Wait to ensure task is not executed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(counter, 0); // No tasks should execute
}

// Test submitting tasks after the loop is stopped
TEST_F(EventLoopTest, StressTest) {
    auto loop = event_loop::make();

    std::vector<std::thread> workers;
    std::atomic<int> counter = 0;
    for (int i = 0; i < 10; ++i) {
        workers.push_back(std::thread([&loop, &counter]() {
            for (int i = 0; i < 10000; ++i) {
                loop->submit([&]() { dummy_task(counter); });
            }
        }));
    }

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    loop->stop();

    ASSERT_EQ(counter, 10000 * 10); // No tasks should execute
}

TEST_F(EventLoopTest, CurrentExecutor) {
    auto loop = event_loop::make();
    std::atomic<int> counter = 0;
    std::atomic<int> flag = 0;

    walle::core::wait_group wg;
    wg.add(1);

    loop->submit([&]() {
        EXPECT_TRUE(loop.get() == current_executor::get());
        dummy_task(counter);
        current_executor::get()->submit([&]() {
            dummy_task(counter);
            current_executor::get()->submit([&]() {
                dummy_task(counter);
                wg.done();
            });
        });
    });

    wg.wait();
    // Wait for the task to execute
    loop->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(counter, 3);
}
