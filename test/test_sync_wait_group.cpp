#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <thread>
#include <vector>
#include <walle/sync/wait_group.hpp>

using wait_group = walle::sync::wait_group;

TEST_CASE("WaitGroup.JustWorks") {
    wait_group wg;

    wg.add(1);
    wg.done();
    wg.wait();
}

TEST_CASE("WaitGroup.InitZero") {
    wait_group wg;
    wg.wait();
}

TEST_CASE("WaitGroup.AddCount") {
    wait_group wg;

    wg.add(7);

    for (size_t i = 0; i < 7; ++i) {
        wg.done();
    }

    wg.wait();
}

TEST_CASE("WaitGroup.Wait") {
    wait_group wg;
    bool ready = false;

    wg.add(1);

    std::thread producer([&] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ready = true;
        wg.done();
    });

    wg.wait();
    REQUIRE(ready);

    producer.join();
}

TEST_CASE("WaitGroup.MultiWait") {
    std::vector<std::thread> threads;

    wait_group wg;
    std::atomic<size_t> work {0};

    static const size_t kWorkers = 3;
    static const size_t kWaiters = 4;

    wg.add(kWorkers);

    for (size_t i = 0; i < kWaiters; ++i) {
        threads.emplace_back([&] {
            wg.wait();
            REQUIRE(work.load() == kWorkers);
        });
    }

    for (size_t i = 1; i <= kWorkers; ++i) {
        threads.emplace_back([&, i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(256 * i));
            ++work;
            wg.done();
        });
    }

    for (auto&& t : threads) {
        t.join();
    }
}

TEST_CASE("WaitGroup.BlockingWait") {
    wait_group wg;

    static const size_t kWorkers = 3;
    std::vector<std::thread> workers;
    std::atomic<size_t> work = 0;

    wg.add(kWorkers);

    for (size_t i = 0; i < kWorkers; ++i) {
        workers.emplace_back([&] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ++work;
            wg.done();
        });
    }

    // You would replace this timer with a suitable mechanism to check elapsed time
    auto start = std::chrono::high_resolution_clock::now();
    wg.wait();
    auto end = std::chrono::high_resolution_clock::now();
    [[maybe_unused]] auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // TODO : optimize and make this better.
    // REQUIRE(duration.count() < 100);

    for (auto& t : workers) {
        t.join();
    }
}

TEST_CASE("WaitGroup.Cyclic") {
    wait_group wg;

    for (size_t i = 0; i < 4; ++i) {
        bool flag = false;

        wg.add(1);

        std::thread worker([&] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            flag = true;
            wg.done();
        });

        wg.wait();

        REQUIRE(flag);

        worker.join();
    }
}
