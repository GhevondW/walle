#pragma once

#include <cstddef>
#include <memory>
#include <new>
#include <utility>

namespace walle::core {

template <typename T, std::size_t Size, std::size_t Align>
class fast_pimpl {
public:
    template <typename... Args>
    explicit fast_pimpl(Args&&... args);

    fast_pimpl(const fast_pimpl& other);
    fast_pimpl(fast_pimpl&& other) noexcept;

    fast_pimpl& operator=(const fast_pimpl& other);
    fast_pimpl& operator=(fast_pimpl&& other) noexcept;

    ~fast_pimpl() noexcept;

    const T& data_ref() const noexcept {
        return *data();
    }

    T& data_ref() noexcept {
        return *data();
    }

    T* data() noexcept {
        return std::launder(reinterpret_cast<T*>(&_storage));
    }

    const T* data() const noexcept {
        return std::launder(reinterpret_cast<const T*>(&_storage));
    }

private:
    alignas(Align) std::byte _storage[Size];
};

template <typename T, std::size_t Size, std::size_t Align>
template <typename... Args>
fast_pimpl<T, Size, Align>::fast_pimpl(Args&&... args)
    : _storage() {
    new (data()) T(std::forward<Args>(args)...);
}

template <typename T, std::size_t Size, std::size_t Align>
fast_pimpl<T, Size, Align>::fast_pimpl(const fast_pimpl& other)
    : _storage() {
    new (data()) T(other.data_ref());
}

template <typename T, std::size_t Size, std::size_t Align>
fast_pimpl<T, Size, Align>::fast_pimpl(fast_pimpl&& other) noexcept
    : _storage() {
    new (data()) T(std::move(other.data_ref()));
}

template <typename T, std::size_t Size, std::size_t Align>
fast_pimpl<T, Size, Align>& fast_pimpl<T, Size, Align>::operator=(const fast_pimpl& other) {
    if (std::addressof(other) == this) {
        return *this;
    }

    data_ref() = other.data_ref();
    return *this;
}

template <typename T, std::size_t Size, std::size_t Align>
fast_pimpl<T, Size, Align>& fast_pimpl<T, Size, Align>::operator=(fast_pimpl&& other) noexcept {
    if (std::addressof(other) == this) {
        return *this;
    }

    data_ref() = std::move(other.data_ref());
    return *this;
}

template <typename T, std::size_t Size, std::size_t Align>
fast_pimpl<T, Size, Align>::~fast_pimpl() noexcept {
    static_assert(Size != 0);
    static_assert(sizeof(T) == Size);
    static_assert(alignof(T) == Align);

    std::destroy_at(data());
}

} // namespace walle::core
