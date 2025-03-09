#include <gtest/gtest.h>
#include <iostream>
#include <walle/asymtx/basic_matrix.hpp>
#include <walle/asymtx/compute_event_awaitable.hpp>
#include <walle/asymtx/go.hpp>
#include <walle/asymtx/mutex.hpp>
#include <walle/asymtx/sync_spawn.hpp>
#include <walle/asymtx/sync_task.hpp>
#include <walle/asymtx/task.hpp>
#include <walle/core/single_shot_event.hpp>
#include <walle/exec/event_loop.hpp>

#include <walle/exec/thread_pool.hpp>

#include <boost/compute/algorithm/accumulate.hpp>
#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/types/builtin.hpp>

#include <iostream>

#define WALLE_LOG(msg) std::cout << "[LOG] " << msg << std::endl;

namespace compute = boost::compute;

namespace {

struct rgba_pixel_t {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;

    std::string to_string() {
        return std::string("[") + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " +
               std::to_string(a) + "]";
    }
};

} // namespace

TEST(asymtx_compute_task, just_works) {
    // get the default device
    compute::device device = compute::system::default_device();

    // create a context for the device
    compute::context context(device);

    // setup input arrays
    float a[] = {1, 2, 3, 4};
    float b[] = {5, 6, 7, 8};

    // make space for the output
    float c[] = {0, 0, 0, 0};

    // create memory buffers for the input and output
    compute::buffer buffer_a(context, 4 * sizeof(float));
    compute::buffer buffer_b(context, 4 * sizeof(float));
    compute::buffer buffer_c(context, 4 * sizeof(float));

    // source code for the add kernel
    const char source[] = "__kernel void add(__global const float *a,"
                          "                  __global const float *b,"
                          "                  __global float *c)"
                          "{"
                          "    const uint i = get_global_id(0);"
                          "    c[i] = a[i] + b[i];"
                          "}";

    // create the program with the source
    compute::program program = compute::program::create_with_source(source, context);

    // compile the program
    program.build();

    // create the kernel
    compute::kernel kernel(program, "add");

    // set the kernel arguments
    kernel.set_arg(0, buffer_a);
    kernel.set_arg(1, buffer_b);
    kernel.set_arg(2, buffer_c);

    // create a command queue
    compute::command_queue queue(context, device);

    // write the data from 'a' and 'b' to the device
    queue.enqueue_write_buffer(buffer_a, 0, 4 * sizeof(float), a);
    queue.enqueue_write_buffer(buffer_b, 0, 4 * sizeof(float), b);

    // run the add kernel
    queue.enqueue_1d_range_kernel(kernel, 0, 4, 0);

    // transfer results back to the host array 'c'
    auto event = queue.enqueue_read_buffer(buffer_c, 0, 4 * sizeof(float), c);

    event.wait();

    // print out results in 'c'
    std::cout << "c: [" << c[0] << ", " << c[1] << ", " << c[2] << ", " << c[3] << "]" << std::endl;
}

walle::asymtx::task_t<std::vector<float>>
calculate_first_vector(walle::asymtx::scheduler_t& sched, compute::context& context, compute::command_queue& queue) {
    // setup input arrays
    float a[] = {1, 2, 3, 4};
    float b[] = {5, 6, 7, 8};

    // make space for the output
    std::vector<float> result {0, 0, 0, 0};

    // create memory buffers for the input and output
    compute::buffer buffer_a(context, 4 * sizeof(float));
    compute::buffer buffer_b(context, 4 * sizeof(float));
    compute::buffer buffer_c(context, 4 * sizeof(float));

    // source code for the add kernel
    const char source[] = "__kernel void add(__global const float *a,"
                          "                  __global const float *b,"
                          "                  __global float *c)"
                          "{"
                          "    const uint i = get_global_id(0);"
                          "    c[i] = a[i] + b[i];"
                          "}";

    // create the program with the source
    compute::program program = compute::program::create_with_source(source, context);

    // compile the program
    program.build();

    // create the kernel
    compute::kernel kernel(program, "add");

    // set the kernel arguments
    kernel.set_arg(0, buffer_a);
    kernel.set_arg(1, buffer_b);
    kernel.set_arg(2, buffer_c);

    // write the data from 'a' and 'b' to the device
    queue.enqueue_write_buffer(buffer_a, 0, 4 * sizeof(float), a);
    queue.enqueue_write_buffer(buffer_b, 0, 4 * sizeof(float), b);

    // run the add kernel
    queue.enqueue_1d_range_kernel(kernel, 0, 4, 0);

    // transfer results back to the host array 'c'
    auto event = queue.enqueue_read_buffer(buffer_c, 0, 4 * sizeof(float), result.data());

    std::cout << "[MAIN_LOG] : Event Wait: " << std::this_thread::get_id() << std::endl;
    co_await walle::asymtx::compute_event_awaitable_t {std::move(event)};
    std::cout << "[MAIN_LOG] : Event End: " << std::this_thread::get_id() << std::endl;

    std::cout << "c: [" << result[0] << ", " << result[1] << ", " << result[2] << ", " << result[3] << "]" << std::endl;

    co_return result;
}

TEST(asymtx_compute_task, just_works_with_asymtx) {
    using namespace walle;

    WALLE_LOG("Starting asymtx_compute_task...");

    exec::thread_pool pool(6);
    asymtx::scheduler_t scheduler(pool);

    compute::device device = compute::system::default_device();
    compute::context context(device);
    compute::command_queue queue(context, device);

    WALLE_LOG("Device : " + device.name());

    WALLE_LOG("Thread pool and scheduler initialized.");

    auto main_task_handle = asymtx::sync_spawn(
        scheduler,
        [](asymtx::scheduler_t& sched, compute::context& context, compute::command_queue& queue) -> asymtx::task_t<> {
            std::cout << "Main task started on thread: " << std::this_thread::get_id() << std::endl;

            auto first_task = walle::asymtx::go(sched, calculate_first_vector(sched, context, queue));
            auto second_task = walle::asymtx::go(sched, calculate_first_vector(sched, context, queue));

            auto a = co_await first_task.get();
            auto b = co_await second_task.get();

            for (int i = 0; i < a.size(); ++i) {
                std::cout << "Value : " << a[i] << ", " << b[i] << std::endl;
            }
        }(scheduler, context, queue));

    WALLE_LOG("Waiting for main task to complete...");
    main_task_handle.blocking_join();

    WALLE_LOG("Stopping thread pool...");
    pool.stop();
    WALLE_LOG("Thread pool stopped successfully.");
}
