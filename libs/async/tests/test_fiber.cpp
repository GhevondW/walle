#include <gtest/gtest.h>

#include <walle/core/defer.hpp>
#include <walle/core/wait_group.hpp>

#include <walle/exec/thread_pool.hpp>

#include <walle/async/go.hpp>
#include <walle/async/yield.hpp>

TEST(async_test_fiber, just_works) {
    walle::exec::thread_pool pool(4);
    walle::core::wait_group wg;

    for (int i = 0; i < 1024; ++i) {
        wg.add();
        walle::async::go(&pool, [&wg]() {
            walle::core::defer_t defer([&wg]() { wg.done(); });

            for (int i = 0; i < 16; ++i) {
                walle::async::yield();
            }
        });
    }

    wg.wait();
    pool.stop();
}
