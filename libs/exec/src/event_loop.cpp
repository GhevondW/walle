#include "walle/exec/event_loop.hpp"
#include <list>
#include <mutex>
#include <thread>
#include <utility>

#include "walle/exec/current_executor.hpp"
#include "walle/exec/executor.hpp"
#include "walle/exec/thread_pool.hpp"

namespace walle::exec {

event_loop::event_loop(private_t)
    : _worker()
    , _tasks()
    , _state(state_e::k_stopped)
    , _mtx()
    , _cv() {}

std::shared_ptr<event_loop> event_loop::make() {
    return std::make_shared<event_loop>(private_t {});
}

event_loop::~event_loop() {
    // TODO think about this, the stop() might throw an exception
    // but the destructor is noexcept
    // stop();
}

// TODO think about pushing in done event loop
bool event_loop::submit(task_t task) {
    {
        std::lock_guard lock(_mtx);
        if (_state == state_e::k_stopped) {
            return false;
        }

        _tasks.push_back(std::move(task));
    }
    _cv.notify_one();
    return true;
}

void event_loop::start() {
    std::lock_guard lock(_mtx);
    if (_state == state_e::k_running) {
        return;
    }

    _worker = std::thread(&event_loop::loop, this);
    _state = state_e::k_running;
}

void event_loop::stop() {
    std::weak_ptr<event_loop> self = shared_from_this();
    submit([self]() {
        if (auto ptr = self.lock()) {
            ptr->_state = state_e::k_stopped;
        }
    });

    if (_worker.joinable()) {
        _worker.join();
    }
}

executor_i::state_e event_loop::state() const noexcept {
    std::lock_guard lock(_mtx);
    return _state;
}

void event_loop::loop() {
    const current_executor::scope_guard scope_guard(this);

    std::list<task_t> current_tasks;

    for (;;) {
        {
            std::unique_lock lock(_mtx);
            while (_tasks.empty() && _state == state_e::k_running) {
                _cv.wait(lock);
            }

            if (_state == state_e::k_stopped) {
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

} // namespace walle::exec
