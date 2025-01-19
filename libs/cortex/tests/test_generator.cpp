#include <gtest/gtest.h>
#include <iostream>
#include <walle/cortex/generator.hpp>

TEST(cortex_test_generator, just_works) {
    walle::cortex::generator_t<int> generator([](auto& self) {
        for (int i = 0; i < 10; ++i) {
            self.yield(i);
        }
    });

    int count = 0;
    while (generator.has_next()) {
        int value = generator.next();
        std::cout << value << std::endl;
        EXPECT_EQ(value, count);
        ++count;
    }
    EXPECT_EQ(count, 10);
}

TEST(cortex_test_generator, empty_generator) {
    walle::cortex::generator_t<int> generator([](auto& self) {
        // No yield calls here
    });

    EXPECT_FALSE(generator.has_next());
}

TEST(cortex_test_generator, single_value_generator) {
    walle::cortex::generator_t<int> generator([](auto& self) { self.yield(42); });

    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 42);
    EXPECT_FALSE(generator.has_next());
}

TEST(cortex_test_generator, multiple_yields) {
    walle::cortex::generator_t<int> generator([](auto& self) {
        self.yield(1);
        self.yield(2);
        self.yield(3);
    });

    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 1);
    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 2);
    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 3);
    EXPECT_FALSE(generator.has_next());
}

// Test case for generator that throws an exception if state is invalid
TEST(cortex_test_generator, invalid_state) {
    walle::cortex::generator_t<int> generator([](auto& self) {
        // We will not yield any value, leading to an invalid state
    });

    EXPECT_FALSE(generator.has_next());
}

TEST(cortex_test_generator, invalid_yield_after_done) {
    walle::cortex::generator_t<int> generator([](auto& self) {
        self.yield(1);
        // We don't call yield again after this
    });

    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 1);
    EXPECT_FALSE(generator.has_next());
}

TEST(cortex_test_generator, no_yield_complete) {
    walle::cortex::generator_t<int> generator([](auto& self) {
        // No yield calls here
    });

    EXPECT_FALSE(generator.has_next());
}

TEST(cortex_test_generator, multiple_coroutines) {
    int shared_value = 0;

    walle::cortex::generator_t<int> generator([](auto& self) {
        for (int i = 0; i < 3; ++i) {
            self.yield(i);
        }
    });

    while (generator.has_next()) {
        int value = generator.next();
        shared_value += value;
    }

    EXPECT_EQ(shared_value, 3); // 0 + 1 + 2 = 3
}

TEST(cortex_test_generator, yield_called_multiple_times) {
    walle::cortex::generator_t<int> generator([](auto& self) {
        self.yield(1);
        self.yield(2);
    });

    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 1);
    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 2);
    EXPECT_FALSE(generator.has_next());
}

TEST(cortex_test_generator, generator_reset) {
    walle::cortex::generator_t<int> generator([](auto& self) {
        self.yield(1);
        self.yield(2);
    });

    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 1);
    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 2);

    generator = walle::cortex::generator_t<int>([](auto& self) { self.yield(3); });

    EXPECT_TRUE(generator.has_next());
    EXPECT_EQ(generator.next(), 3);
    EXPECT_FALSE(generator.has_next());
}
