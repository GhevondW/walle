#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <optional>
#include <thread>
#include <vector>
#include <walle/core/mpmc_unbounded_blocking_queue.hpp>
#include <walle/core/wait_group.hpp>

TEST(core_test_unbounded_blocking_queue, push_and_pop_single_thread) {
    walle::core::mpmc_unbounded_blocking_queue<int> queue;

    EXPECT_TRUE(queue.push(10));
    EXPECT_TRUE(queue.push(20));

    auto value1 = queue.wait_and_pop();
    EXPECT_TRUE(value1.has_value());
    EXPECT_EQ(value1.value(), 10);

    auto value2 = queue.wait_and_pop();
    EXPECT_TRUE(value2.has_value());
    EXPECT_EQ(value2.value(), 20);
}

TEST(core_test_unbounded_blocking_queue, push_move) {
    walle::core::mpmc_unbounded_blocking_queue<std::string> queue;
    std::string value = "hello";

    EXPECT_TRUE(queue.push(std::move(value)));

    auto result = queue.wait_and_pop();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "hello");
}

TEST(core_test_unbounded_blocking_queue, wait_and_pop_when_empty) {
    walle::core::mpmc_unbounded_blocking_queue<int> queue;

    std::thread producer([&queue]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        queue.push(42);
    });

    auto result = queue.wait_and_pop();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);

    producer.join();
}

TEST(core_test_unbounded_blocking_queue, wake_and_done_prevents_new_pushes) {
    walle::core::mpmc_unbounded_blocking_queue<int> queue;

    queue.push(10);
    queue.wake_and_done();

    EXPECT_FALSE(queue.push(20)); // Cannot push after wake_and_done

    auto value1 = queue.wait_and_pop();
    EXPECT_TRUE(value1.has_value());
    EXPECT_EQ(value1.value(), 10);

    auto value2 = queue.wait_and_pop();
    EXPECT_FALSE(value2.has_value()); // Queue is closed
}

TEST(core_test_unbounded_blocking_queue, multiple_producers_consumers) {
    walle::core::mpmc_unbounded_blocking_queue<int> queue;

    constexpr std::size_t num_producers = 11;
    constexpr std::size_t num_consumers = 12;
    constexpr std::size_t items_per_producer = 130;

    std::atomic<std::size_t> result_items = 0;
    constexpr std::size_t ites_count = items_per_producer * num_producers;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    std::vector<int> results;

    std::mutex results_mutex;

    // Create producers
    for (std::size_t i = 0; i < num_producers; ++i) {
        producers.emplace_back([&queue, i]() {
            for (std::size_t j = 0; j < items_per_producer; ++j) {
                queue.push(i);
            }
        });
    }

    // Create consumers
    for (std::size_t i = 0; i < num_consumers; ++i) {
        consumers.emplace_back([&queue, &results, &results_mutex, &result_items]() {
            while (true) {
                auto value = queue.wait_and_pop();
                if (!value.has_value() && queue.is_closed()) {
                    break;
                }
                {
                    std::lock_guard<std::mutex> lock(results_mutex);
                    results.push_back(value.value());
                    ++result_items;
                }
            }
        });
    }

    // Wait for producers to finish
    for (auto& producer : producers) {
        producer.join();
    }

    queue.wake_and_done();

    // Wait for consumers to finish
    for (auto& consumer : consumers) {
        consumer.join();
    }

    EXPECT_EQ(result_items.load(), ites_count);
    EXPECT_EQ(results.size(), ites_count);
}
