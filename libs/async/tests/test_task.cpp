#include <gtest/gtest.h>

#include <iostream>
#include <walle/async/go.hpp>
#include <walle/async/task_handle.hpp>
#include <walle/async/this_task.hpp>
#include <walle/core/defer.hpp>
#include <walle/core/wait_group.hpp>
#include <walle/exec/thread_pool.hpp>

#include <vector>

#define TID std::this_thread::get_id()

TEST(async_test_fiber, api_just_works) {
    walle::exec::thread_pool pool(4);

    std::cout << "Main thread : " << TID << std::endl;
    auto main_task = walle::async::go(&pool, []() {
        std::cout << "Main task start id : " << TID << std::endl;

        std::vector<walle::async::task_handle_t> tasks;
        for (int i = 0; i < 10; ++i) {
            tasks.push_back(walle::async::go([i]() { std::cout << "Child " << i << " : " << TID << std::endl; }));
        }

        for (auto& task : tasks) {
            task.join();
        }

        std::cout << "Main task end ..." << std::endl;
    });

    main_task.blocking_join();

    pool.stop();
}
