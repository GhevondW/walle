#include <catch2/catch_test_macros.hpp>

#include <string>
#include <walle/core/fast_pimpl.hpp>

#include <common/sample_pimpl.hpp>

using namespace std::chrono_literals;

TEST_CASE("walle::core::fast_pimpl.just_works") {
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
        REQUIRE(counter == 0);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl(&counter);
        REQUIRE(counter == 1);
    }
    REQUIRE(counter == 2);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        REQUIRE(counter == 3);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2(&counter);
        REQUIRE(counter == 4);

        impl1 = impl2;
        REQUIRE(counter == 5);
    }
    REQUIRE(counter == 7);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        REQUIRE(counter == 8);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2(std::move(impl1));
        REQUIRE(counter == 9);
    }
    REQUIRE(counter == 11);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        REQUIRE(counter == 12);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2(
            static_cast<const walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)>&>(impl1));

        REQUIRE(counter == 13);
    }
    REQUIRE(counter == 15);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        REQUIRE(counter == 16);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2(impl1);
        REQUIRE(counter == 17);
    }
    REQUIRE(counter == 19);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        REQUIRE(counter == 20);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2 = impl1;
        REQUIRE(counter == 21);
    }
    REQUIRE(counter == 23);

    {
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl1(&counter);
        REQUIRE(counter == 24);
        walle::core::fast_pimpl<sample, sizeof(sample), alignof(sample)> impl2 = std::move(impl1);
        REQUIRE(counter == 25);
    }
    REQUIRE(counter == 27);
}

TEST_CASE("walle::core::fast_pimpl.sample") {
    test::common::sample_pimpl pimpl("foo");
    REQUIRE(std::string(pimpl.foo()) == "foo");
}

// TODO : add tests
