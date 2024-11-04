#include <atomic>
#include <cstddef>
#include <gtest/gtest.h>

#include <mutex>
#include <thread>
#include <vector>
#include <walle/core/unbounded_blocking_queue.hpp>

TEST(UnboundedBlockingQueueTest, JustWorks) {
    walle::core::unbounded_blocking_queue<int> queue;
    for (int i = 0; i < 1000; ++i) {
        queue.push(i);
    }

    for (int i = 0; i < 1000; ++i) {
        EXPECT_EQ(queue.wait_and_pop(), i);
    }
}

TEST(UnboundedBlockingQueueTest, ProduceAndConsume) {
    walle::core::unbounded_blocking_queue<int> queue;

    std::vector<std::thread> pros;
    std::vector<std::thread> cons;

    int p_count = 100;
    int c_count = 10;

    for (int i = 0; i < p_count; ++i) {
        pros.emplace_back([&queue]() {
            for (int value = 0; value < 10; ++value) {
                queue.push(value);
            }
        });
    }

    std::mutex mtx;
    std::vector<int> res;
    for (int i = 0; i < c_count; ++i) {
        cons.emplace_back([&queue, &mtx, &res]() {
            while (true) {
                if (queue.is_done()) {
                    break;
                }

                {
                    std::lock_guard lock(mtx);
                    res.push_back(queue.wait_and_pop());
                }
            }
        });
    }

    for (auto& thr : pros) {
        thr.join();
    }

    queue.wake_and_done();

    for (auto& thr : cons) {
        thr.join();
    }

    std::cout << "Count : " << res.size() << std::endl;
}
