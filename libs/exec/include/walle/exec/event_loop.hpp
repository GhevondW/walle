#pragma once

#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>

#include <walle/exec/executor.hpp>

namespace walle::exec {

class event_loop
    : public std::enable_shared_from_this<event_loop>
    , public executor_i {
private:
    struct private_t {
        explicit private_t() = default;
    };

public:
    event_loop(private_t);

    static std::shared_ptr<event_loop> make();

    event_loop(const event_loop&) = delete;
    event_loop(event_loop&&) noexcept = delete;
    event_loop& operator=(const event_loop&) = delete;
    event_loop& operator=(event_loop&&) noexcept = delete;
    ~event_loop() override;

    void submit(task_t task) override;
    void stop();

private:
    void loop();

    std::thread _worker;
    std::list<task_t> _tasks; // guarded my _mtx
    bool _done;
    std::mutex _mtx;
    std::condition_variable _cv;
};

} // namespace walle::exec
