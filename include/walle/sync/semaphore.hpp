#pragma once

#include <cstdlib>
#include <mutex>
#include <condition_variable>

namespace walle::sync {

// TODO : make this templated, maybe it is possible to use this class
//        in fiber context too with fiber mutex and cv.
class semaphore {
public:
    explicit semaphore(std::size_t tokens)
        :_tokens(tokens)
        ,_mtx()
        ,_cv()
    {}

    void release();
    void acquire();

private:
    std::size_t _tokens;
    std::mutex _mtx;
    std::condition_variable _cv;
};
    
}
