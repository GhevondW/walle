#include "blocking_parking_lot.hpp"
#include <mutex>

namespace walle::async {

void blocking_parking_lot_t::park() {
    auto this_id = std::this_thread::get_id();

    std::unique_lock lock(_mtx);
    if (_closed) {
        return;
    }

    _queue.push_back(this_id);
    _parked.insert(this_id);

    while (_closed == false && _parked.contains(this_id)) {
        _cv.wait(lock);
    }
}

void blocking_parking_lot_t::unpark_one() {
    {
        std::lock_guard lock(_mtx);
        if (_queue.empty()) {
            return;
        }

        auto front = _queue.front();
        _queue.pop_front();
        _parked.erase(front);
    }
    _cv.notify_all();
}

void blocking_parking_lot_t::unpark_all() {
    {
        std::lock_guard lock(_mtx);
        _queue.clear();
        _parked.clear();
    }
    _cv.notify_all();
}

void blocking_parking_lot_t::wake_and_close() {
    {
        std::lock_guard lock(_mtx);
        _closed = true;
        _queue.clear();
        _parked.clear();
    }
    _cv.notify_all();
}

} // namespace walle::async
