#include <coroutine>
#include <gtest/gtest.h>

#include <walle/std/resumable.hpp>

static int Global = 0;

walle::std::resumable hello_world() {
    EXPECT_EQ(Global, 1);
    ++Global;
    co_await std::suspend_always {};

    EXPECT_EQ(Global, 3);
    ++Global;
    co_await std::suspend_always {};

    EXPECT_EQ(Global, 5);
    ++Global;
    co_await std::suspend_always {};

    EXPECT_EQ(Global, 7);
    ++Global;
    co_await std::suspend_always {};
}

TEST(StdResumable, JustWorks) {
    auto coro = hello_world();

    EXPECT_EQ(Global, 0);

    ++Global;
    EXPECT_TRUE(coro.resume());
    EXPECT_EQ(Global, 2);

    ++Global;
    EXPECT_TRUE(coro.resume());
    EXPECT_EQ(Global, 4);

    ++Global;
    EXPECT_TRUE(coro.resume());
    EXPECT_EQ(Global, 6);

    ++Global;
    EXPECT_TRUE(coro.resume());
    EXPECT_EQ(Global, 8);
}
