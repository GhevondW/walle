#include <gtest/gtest.h>

#include <cortex/coroutine.hpp>

#include <boost/context/fiber.hpp>

// TEST(Cortex, JustWorks) {
//     // auto coro = cortex::coroutine::create([](auto& self){
//     //     std::cout << "#2" << std::endl;
//     //     self.suspend();
//     //     std::cout << "#4" << std::endl;
//     //     self.suspend();
//     //     std::cout << "#6" << std::endl;
//     // });

//     // std::cout << "#1" << std::endl;
//     // coro.resume();
//     // std::cout << "#3" << std::endl;
//     // coro.resume();
//     // std::cout << "#5" << std::endl;
// }

TEST(Cortex, JustWorksBoost) {
    namespace ctx = boost::context;
    int a;
    ctx::fiber source {[&a](ctx::fiber&& sink) {
        a = 0;
        int b = 1;
        for (;;) {
            sink = std::move(sink).resume();
            int next = a + b;
            a = b;
            b = next;
        }
        return std::move(sink);
    }};
    for (int j = 0; j < 10; ++j) {
        source = std::move(source).resume();
        std::cout << a << " ";
    }
}
