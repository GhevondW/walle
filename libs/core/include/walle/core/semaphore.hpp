#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <mutex>

namespace walle::core {

// TODO : make this templated, maybe it is possible to use this class
//        in fiber context too with fiber mutex and cv.
class semaphore {
public:
    explicit semaphore(std::size_t tokens)
        : _tokens(tokens)
        , _closed(false)
        , _mtx()
        , _cv() {}
    ~semaphore();

    void release();
    void acquire();

private:
    std::size_t _tokens;
    std::atomic<bool> _closed;
    std::mutex _mtx;
    std::condition_variable _cv;
};

} // namespace walle::core
