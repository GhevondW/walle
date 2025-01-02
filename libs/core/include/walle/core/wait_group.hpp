#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <mutex>

#include <walle/core/non_copyable.hpp>
#include <walle/core/non_movable.hpp>

namespace walle::core {

class basic_wait_group
    : non_copyable
    , non_movable {
public:
    basic_wait_group() = default;
    ~basic_wait_group() = default;

    void add(std::size_t count = 1);
    void done(std::size_t count = 1);
    void wait();

private:
    std::size_t _count {0};
    std::mutex _mtx {};
    std::condition_variable _idle {};
};

class wait_group
    : non_copyable
    , non_movable {
public:
    wait_group() = default;
    ~wait_group() = default;

    void add(std::size_t count = 1);
    void done(std::size_t count = 1);
    void wait();

private:
    std::atomic<std::size_t> _count {0};
    std::mutex _mtx {};
    std::condition_variable _idle {};
};

} // namespace walle::core
