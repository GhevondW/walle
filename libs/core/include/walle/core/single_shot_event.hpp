#pragma once

#ifdef WALLE_CORE_SINGLE_SHOT_EVENT_USE_ATOMICS
#include <atomic>
#else
#include <condition_variable>
#include <mutex>
#endif

namespace walle::core {

class single_shot_event_t {
public:
    explicit single_shot_event_t(bool initial_state = false)
        : _event(initial_state) {}
    ~single_shot_event_t() = default;

    void wait() {
#ifdef WALLE_CORE_SINGLE_SHOT_EVENT_USE_ATOMICS
        _event.wait(false, std::memory_order_relaxed);
#else
        std::unique_lock lock(_mtx);
        while (!_event) {
            _cv.wait(lock);
        }
#endif
    }

    // `set` must be called before this object goes out of scope
    void set() {
#ifdef WALLE_CORE_SINGLE_SHOT_EVENT_USE_ATOMICS
        _event.store(true, std::memory_order_relaxed);
        _event.notify_all();
#else
        {
            std::unique_lock lock(_mtx);
            _event = true;
        }
        _cv.notify_all();
#endif
    }

private:
#ifdef WALLE_CORE_SINGLE_SHOT_EVENT_USE_ATOMICS
    std::atomic<bool> _event {false};
#else
    bool _event {false};
    std::mutex _mtx {};
    std::condition_variable _cv {};
#endif
};

} // namespace walle::core
