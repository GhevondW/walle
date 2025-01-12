#include <gtest/gtest.h>
#include <walle/cortex/coroutine_base.hpp>
#include <walle/cortex/error.hpp>

// TODO : add more tests
TEST(cortex_test_coroutine_base, just_works) {
    int global = 1;

    struct my_coroutine final : public walle::cortex::coroutine_base_i {
        my_coroutine(int& gl)
            : coroutine_base_i()
            , _global(gl) {}

        ~my_coroutine() override = default;

    private:
        void flow(walle::cortex::suspend_context_i& self) override {
            std::cout << "coro -> #1" << std::endl;
            EXPECT_EQ(_global++, 1);
            self.suspend();
            std::cout << "coro -> #3" << std::endl;
            EXPECT_EQ(_global++, 3);
            self.suspend();
            std::cout << "coro -> #5" << std::endl;
            EXPECT_EQ(_global++, 5);
        }

    private:
        int& _global;
    };

    auto handle = my_coroutine(global);
    EXPECT_TRUE(handle.is_valid());

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
