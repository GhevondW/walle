#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <walle/core/event_loop.hpp>
#include <walle/core/executor.hpp> // Replace with actual path to executor_i and event_loop headers

using namespace walle::core;

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
    event_loop loop;
    // Verify the event loop initializes without issues
    SUCCEED(); // If no crash happens, the test passes
}

// Test submitting a task and ensuring it executes
TEST_F(EventLoopTest, SubmitsTaskAndExecutes) {
    event_loop loop;
    std::atomic<int> counter = 0;

    loop.submit([&]() { dummy_task(counter); });

    // Wait for the task to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(counter, 1);
}

// Test multiple task submissions
TEST_F(EventLoopTest, ExecutesMultipleTasks) {
    event_loop loop;
    std::atomic<int> counter = 0;

    for (int i = 0; i < 5; ++i) {
        loop.submit([&]() { dummy_task(counter); });
    }

    // Wait for tasks to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(counter, 5);
}

// Test stopping the event loop
TEST_F(EventLoopTest, StopsEventLoop) {
    event_loop loop;
    std::atomic<int> counter = 0;

    loop.submit([&]() { dummy_task(counter); });
    loop.stop();

    // Submit another task after stopping
    loop.submit([&]() { dummy_task(counter); });

    // Wait to ensure tasks are not executed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(counter, 1); // Only the first task should execute
}

// Test destructor cleans up properly
TEST_F(EventLoopTest, DestructorStopsAndCleansUp) {
    std::atomic<int> counter = 0;

    {
        event_loop loop;
        loop.submit([&]() { dummy_task(counter); });
    } // Event loop goes out of scope and destructor is called

    // Give some time for destructor cleanup
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(counter, 1); // Task should have executed before destruction
}

// Test edge case: no tasks submitted
TEST_F(EventLoopTest, NoTasksSubmitted) {
    event_loop loop;
    loop.stop();
    SUCCEED(); // Ensure no crash occurs
}

// Test submitting tasks after the loop is stopped
TEST_F(EventLoopTest, SubmitAfterStopDoesNothing) {
    event_loop loop;
    loop.stop();

    std::atomic<int> counter = 0;
    loop.submit([&]() { dummy_task(counter); });

    // Wait to ensure task is not executed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(counter, 0); // No tasks should execute
}

// Test submitting tasks after the loop is stopped
TEST_F(EventLoopTest, StressTest) {
    event_loop loop;

    std::vector<std::thread> workers;
    std::atomic<int> counter = 0;
    for (int i = 0; i < 10; ++i) {
        workers.push_back(std::thread([&loop, &counter]() {
            for (int i = 0; i < 1000000; ++i) {
                loop.submit([&]() { dummy_task(counter); });
            }
        }));
    }

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    loop.stop();

    // Wait to ensure task is not executed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(counter, 1000000 * 10); // No tasks should execute
}
