#pragma once

#ifdef WALLE_CORE_SINGLE_SHOT_EVENT_USE_ATOMICS
#include <atomic>
#define WALLE_CORE_SINGLE_SHOT_EVENT_NOEXCEPT noexcept
#define WALLE_CORE_SINGLE_SHOT_EVENT_CLASS_NAME atomic_single_shot_event_t
#else
#include <condition_variable>
#include <mutex>
#define WALLE_CORE_SINGLE_SHOT_EVENT_NOEXCEPT
#define WALLE_CORE_SINGLE_SHOT_EVENT_CLASS_NAME single_shot_event_t
#endif

namespace walle::core {

// TODO : think about memory order, right now this is ok
class WALLE_CORE_SINGLE_SHOT_EVENT_CLASS_NAME {
public:
    explicit WALLE_CORE_SINGLE_SHOT_EVENT_CLASS_NAME(bool initial_state = false) noexcept
        : _event(initial_state) {}
    ~WALLE_CORE_SINGLE_SHOT_EVENT_CLASS_NAME() = default;

    void wait() WALLE_CORE_SINGLE_SHOT_EVENT_NOEXCEPT {
#ifdef WALLE_CORE_SINGLE_SHOT_EVENT_USE_ATOMICS
        _event.wait(false);
#else
        std::unique_lock lock(_mtx);
        while (!_event) {
            _cv.wait(lock);
        }
#endif
    }

    // `set` must be called before this object goes out of scope
    void set() WALLE_CORE_SINGLE_SHOT_EVENT_NOEXCEPT {
#ifdef WALLE_CORE_SINGLE_SHOT_EVENT_USE_ATOMICS
        _event.store(true);
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
