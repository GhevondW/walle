#include <common/sample_pimpl.hpp>
#include <gtest/gtest.h>
#include <string>
#include <walle/core/fast_pimpl.hpp>

using namespace std::chrono_literals;

TEST(FastPimplTest, JustWorks) {
    int counter = 0;
    struct sample {
        sample(int* cnt)
            : counter(cnt) {
            add_one();
        }

        sample(const sample& other)
            : counter(other.counter) {
            add_one();
        }

        sample(sample&& other) noexcept
            : counter(other.counter) {
            add_one();
        }

        sample& operator=(const sample& other) {
            counter = other.counter;
            add_one();
            return *this;
        }

        sample& operator=(sample&& other) noexcept {
            counter = other.counter;
            add_one();
            return *this;
        }

        ~sample() {
            add_one();
        }

        void add_one() {
            *counter = *counter + 1;
        }

        int* counter = nullptr;
    };

    {
        EXPECT_EQ(counter, 0);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl(&counter);
        EXPECT_EQ(counter, 1);
    }
    EXPECT_EQ(counter, 2);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        EXPECT_EQ(counter, 3);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2(&counter);
        EXPECT_EQ(counter, 4);

        impl1 = impl2;
        EXPECT_EQ(counter, 5);
    }
    EXPECT_EQ(counter, 7);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        EXPECT_EQ(counter, 8);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2(std::move(impl1));
        EXPECT_EQ(counter, 9);
    }
    EXPECT_EQ(counter, 11);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        EXPECT_EQ(counter, 12);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2(
            static_cast<const walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)>&>(impl1));

        EXPECT_EQ(counter, 13);
    }
    EXPECT_EQ(counter, 15);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        EXPECT_EQ(counter, 16);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2(impl1);
        EXPECT_EQ(counter, 17);
    }
    EXPECT_EQ(counter, 19);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        EXPECT_EQ(counter, 20);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2 = impl1;
        EXPECT_EQ(counter, 21);
    }
    EXPECT_EQ(counter, 23);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        EXPECT_EQ(counter, 24);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2 = std::move(impl1);
        EXPECT_EQ(counter, 25);
    }
    EXPECT_EQ(counter, 27);
}

TEST(FastPimplTest, Sample) {
    test::common::sample_pimpl pimpl("foo");
    EXPECT_EQ(std::string(pimpl.foo()), "foo");
}
