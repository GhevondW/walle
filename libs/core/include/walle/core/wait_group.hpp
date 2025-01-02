#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <mutex>

namespace walle::core {

class basic_wait_group {
public:
    basic_wait_group() = default;
    ~basic_wait_group() = default;
    basic_wait_group(const basic_wait_group& other) = delete;
    basic_wait_group(basic_wait_group&& other) = delete;

    basic_wait_group& operator=(const basic_wait_group& other) = delete;
    basic_wait_group& operator=(basic_wait_group&& other) = delete;

    void add(std::size_t count = 1);
    void done(std::size_t count = 1);
    void wait();

private:
    std::size_t _count {0};
    std::mutex _mtx {};
    std::condition_variable _idle {};
};

class wait_group {
public:
    wait_group() = default;
    ~wait_group() = default;
    wait_group(const wait_group& other) = delete;
    wait_group(wait_group&& other) noexcept = delete;

    wait_group& operator=(const wait_group& other) = delete;
    wait_group& operator=(wait_group&& other) noexcept = delete;

    void add(std::size_t count = 1);
    void done(std::size_t count = 1);
    void wait();

private:
    std::atomic<std::size_t> _count {0};
    std::mutex _mtx {};
    std::condition_variable _idle {};
};

} // namespace walle::core
