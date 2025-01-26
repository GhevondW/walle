#include "blocking_parking_lot.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

namespace walle::async {

TEST(BlockingParkingLotTest, ParkAndUnparkOne) {
    blocking_parking_lot_t parking_lot;
    std::atomic<bool> thread_started {false};
    std::atomic<bool> unparked {false};
    std::thread t([&]() {
        thread_started.store(true);
        parking_lot.park();
        unparked.store(true);
    });

    while (!thread_started.load()) {
        std::this_thread::yield();
    }

    // This test can break but ok for now
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::this_thread::yield();
    EXPECT_FALSE(unparked.load());

    while (!unparked) {
        parking_lot.unpark_one();
    }

    t.join();
    EXPECT_TRUE(unparked.load());
    parking_lot.wake_and_close();
}

TEST(BlockingParkingLotTest, WakeAndCloseBlocksFurtherParks) {
    blocking_parking_lot_t parking_lot;
    parking_lot.wake_and_close();

    std::atomic<bool> thread_exited {false};
    std::thread t([&]() {
        parking_lot.park();
        thread_exited.store(true);
    });

    t.join();
    EXPECT_TRUE(thread_exited.load());
}

} // namespace walle::async
