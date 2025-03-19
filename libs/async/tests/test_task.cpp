#include <gtest/gtest.h>

#include <iostream>
#include <walle/async/go.hpp>
#include <walle/async/task_handle.hpp>
#include <walle/async/this_task.hpp>
#include <walle/core/defer.hpp>
#include <walle/core/wait_group.hpp>
#include <walle/exec/thread_pool.hpp>

#include <vector>

TEST(async_test_fiber, api_just_works) {
    walle::exec::thread_pool pool(4);

    auto main_task = walle::async::go(&pool, []() {
        std::cout << "Main task start ..." << std::endl;

        std::vector<walle::async::task_handle> tasks;
        for (int i = 0; i < 10; ++i) {
            tasks.push_back(walle::async::go([i]() { std::cout << "Child task " << i << "..." << std::endl; }));
        }

        for (auto& task : tasks) {
            task.join();
        }

        std::cout << "Main task end ..." << std::endl;
    });

    main_task.blocking_join();

    pool.stop();
}

// TEST(async_test_fiber, just_works) {
//     walle::exec::thread_pool pool(4);
//     walle::core::wait_group wg;

//     for (int i = 0; i < 1024; ++i) {
//         wg.add();
//         walle::async::go(pool, [&wg]() {
//             walle::core::defer_t defer([&wg]() { wg.done(); });

//             for (int i = 0; i < 16; ++i) {
//                 walle::async::this_task::yield();
//             }
//         });
//     }

//     wg.wait();
//     pool.stop();

//     walle::async::task_handle handle;
// }
