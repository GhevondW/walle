#include <gtest/gtest.h>

#include <cortex/cortex.hpp>

TEST(cortex_test, just_works) {
    EXPECT_EQ(cortex::foo(), "Hi");
}
