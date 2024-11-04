#include <gtest/gtest.h>
#include <walle/core/coroutine_handle.hpp>

#include <iostream>

TEST(CoroutineHandle, JustWorks) {
    int global = 1;

    auto handle = walle::core::coroutine_handle::create([&global](auto& ctx) {
        std::cout << "coro -> #1" << std::endl;
        EXPECT_EQ(global++, 1);
        ctx.suspend();
        std::cout << "coro -> #3" << std::endl;
        EXPECT_EQ(global++, 3);
        ctx.suspend();
        std::cout << "coro -> #5" << std::endl;
        EXPECT_EQ(global++, 5);
    });

    handle.resume();
    std::cout << "main -> #2" << std::endl;
    EXPECT_EQ(global++, 2);
    EXPECT_FALSE(handle.is_done());
    handle.resume();
    std::cout << "main -> #4" << std::endl;
    EXPECT_EQ(global++, 4);
    EXPECT_FALSE(handle.is_done());
    handle.resume();
    std::cout << "main -> #6" << std::endl;
    EXPECT_EQ(global++, 6);
    EXPECT_TRUE(handle.is_done());
}

TEST(CoroutineHandle, ResumeOnFinishedCoroutine) {
    int global = 0;

    auto handle = walle::core::coroutine_handle::create([&global](auto& ctx) {
        std::cout << "coro -> #1" << std::endl;
        EXPECT_EQ(global, 1);
        ++global;
    });

    EXPECT_EQ(global, 0);
    ++global;
    handle.resume();

    EXPECT_EQ(global, 2);
    EXPECT_THROW(handle.resume(), walle::core::coroutine_handle::resume_on_completed_coroutine_error_t);
}

TEST(CoroutineHandle, Exceptions) {
    int global = 0;

    struct custom_exception : walle::core::error {
        using error::error;
    };

    auto handle = walle::core::coroutine_handle::create([&global](auto& ctx) {
        std::cout << "coro -> #1" << std::endl;
        EXPECT_EQ(++global, 1);
        ctx.suspend();
        throw custom_exception {"my exception"};
    });

    handle.resume();
    std::cout << "main -> #2" << std::endl;
    EXPECT_EQ(++global, 2);
    EXPECT_FALSE(handle.is_done());

    try {
        handle.resume();
    } catch (const custom_exception& e) {
        ++global;
        std::cout << "rethrowed exception" << std::endl;
    }

    EXPECT_TRUE(handle.is_done());
    EXPECT_EQ(global, 3);
}

TEST(CoroutineHandle, DestroyUnfinishedCoroutine) {
    int global = 0;

    struct test_struct {
        test_struct(int& g)
            : gl(g) {}
        ~test_struct() {
            std::cout << "dtor" << std::endl;
            ++gl;
        }
        int& gl;
    };

    {
        auto handle = walle::core::coroutine_handle::create([&global](auto& ctx) {
            std::cout << "coro -> #2" << std::endl;
            test_struct ts(global);
            EXPECT_EQ(global, 1);
            ++global;
            ctx.suspend();

            EXPECT_FALSE(true); // we must not get here
        });

        std::cout << "main -> #1" << std::endl;
        EXPECT_EQ(global, 0);
        ++global;
        handle.resume();

        std::cout << "main -> #3" << std::endl;
        EXPECT_EQ(global, 2);
    }
    std::cout << "main -> #4" << std::endl;
    EXPECT_EQ(global, 3);
}

TEST(CortexNestedExecutionTest, Nested) {
    int counter = 0;

    auto execution_one = walle::core::coroutine_handle::create([&counter](auto& suspender) {
        EXPECT_EQ(++counter, 2);
        std::cout << "Step 2" << '\n';
        suspender.suspend();

        EXPECT_EQ(++counter, 4);
        std::cout << "Step 4" << '\n';
    });

    auto execution_two = walle::core::coroutine_handle::create([&execution_one, &counter](auto& suspender) {
        EXPECT_EQ(++counter, 1);
        std::cout << "Step 1" << '\n';
        execution_one.resume();

        EXPECT_EQ(++counter, 3);
        std::cout << "Step 3" << '\n';
        execution_one.resume();

        auto nested = walle::core::coroutine_handle::create([&counter](auto& suspender) {
            EXPECT_EQ(++counter, 5);
            std::cout << "Step 5" << '\n';
            suspender.suspend();

            EXPECT_EQ(++counter, 7);
            std::cout << "Step 7" << '\n';
        });

        nested.resume();

        EXPECT_EQ(++counter, 6);
        std::cout << "Step 6" << '\n';
        nested.resume();
    });

    execution_two.resume();

    EXPECT_EQ(counter, 7);
}
