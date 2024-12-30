#include "walle/core/thread_pool.hpp"
#include "walle/core/utils/optional.hpp"

namespace walle::core {

thread_pool::thread_pool(std::size_t workers_count)
    : _workers_count(workers_count)
    , _tasks()
    , _workers()
    , _wait_group() {
    if (_workers_count == 0) {
        throw std::invalid_argument {"the workers count can not be zero."};
    }

    init();
}

void thread_pool::submit(task_t task) {
    _wait_group.add();
    _tasks.push(std::move(task));
}

void thread_pool::wait_idle() {
    _wait_group.wait();
}

void thread_pool::stop() {
    _tasks.wake_and_done();
    for (auto& worker : _workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void thread_pool::init() {
    _workers.reserve(_workers_count);
    for (std::size_t i = 0; i < _workers_count; ++i) {
        _workers.emplace_back(&thread_pool::loop, this);
    }
}

void thread_pool::loop() {
    for (;;) {
        auto task = _tasks.wait_and_pop();
        if (!task.has_value()) {
            break;
        }

        // TODO hadle exceptions from task.
        utils::asserted_value(task)();
        _wait_group.done();
    }
}

} // namespace walle::core
