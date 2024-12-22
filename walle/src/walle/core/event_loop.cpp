#include "walle/core/event_loop.hpp"
#include <list>
#include <thread>
#include <utility>

#include "walle/core/utils/current_executor.hpp"

namespace walle::core {

event_loop::event_loop(private_t)
    : _worker()
    , _tasks()
    , _done(false)
    , _mtx()
    , _cv() {
    _worker = std::thread(&event_loop::loop, this);
}

std::shared_ptr<event_loop> event_loop::make() {
    return std::make_shared<event_loop>(private_t{});
}

event_loop::~event_loop() {
    // TODO think about this, the stop() might throw an exception
    // but the destructor is noexcept
    // stop();
}

// TODO think about pushing in done event loop
void event_loop::submit(task_t task) {
    {
        std::lock_guard lock(_mtx);
        _tasks.push_back(std::move(task));
    }
    _cv.notify_one();
}

void event_loop::stop() {
    std::weak_ptr<event_loop> self = shared_from_this();
    submit([self]() { 
        if(auto ptr = self.lock()) {
            ptr->_done = true; 
        }
    });

    if (_worker.joinable()) {
        _worker.join();
    }
}

void event_loop::loop() {
    const utils::current_executor::scope_guard scope_guard(this);

    std::list<task_t> current_tasks;

    for (;;) {
        {
            std::unique_lock lock(_mtx);
            while (_tasks.empty() && !_done) {
                _cv.wait(lock);
            }

            if (_done) {
                return;
            }

            std::swap(current_tasks, _tasks);
        }

        while (!current_tasks.empty()) {
            // TODO handle exceptions ...
            current_tasks.front()();
            current_tasks.pop_front();
        }
        current_tasks.clear();
    }
}

} // namespace walle::core
