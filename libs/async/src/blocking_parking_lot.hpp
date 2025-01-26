#pragma once

#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>
#include <unordered_set>

namespace walle::async {

// Temporary solution, i just use this as a temporary solution for wait queue.
struct blocking_parking_lot_t {
public:
    blocking_parking_lot_t() = default;
    ~blocking_parking_lot_t() noexcept = default;

    void park();
    void unpark_one();
    void unpark_all();
    // // TODO : if needed
    // bool unpark_thread(std::thread::id);

    // must be called before dtor.
    void wake_and_close();

private:
    std::mutex _mtx {};
    std::condition_variable _cv {};
    std::list<std::thread::id> _queue {};
    std::unordered_set<std::thread::id> _parked {};
    bool _closed {false};
};

} // namespace walle::async
