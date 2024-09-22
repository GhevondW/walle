#include <mutex>
#include <walle/sync/semaphore.hpp>

namespace walle::sync {

void semaphore::release() {
    {
        std::lock_guard lock(_mtx);
        ++_tokens;
    }
    _cv.notify_one();
}

void semaphore::acquire() {
    std::unique_lock lock(_mtx);
    while(_tokens == 0) {
        _cv.wait(lock);
    }
    --_tokens;
}

}
