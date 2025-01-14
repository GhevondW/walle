#include <gtest/gtest.h>
#include <walle/cortex/generator.hpp>

// TODO : add more tests
TEST(cortex_test_generator, just_works) {
    walle::cortex::generator_t<int> generator([](auto& self) {
        for (int i = 0; i < 10; ++i) {
            self.yield(i);
        }
    });

    while (generator.has_next()) {
        std::cout << generator.next() << std::endl;
    }
}
