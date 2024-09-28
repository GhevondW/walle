#pragma once

#include <cstddef>
#include <memory>
#include <new>
#include <utility>

namespace walle::core {

template <typename T, std::size_t Size, std::size_t Align>
class fast_pimpl final {
public:
    template <typename... Args>
    explicit fast_pimpl(Args&&... args) noexcept(noexcept(T(std::declval<Args>()...)))
        : _storage() {
        new (as_type()) T(std::forward<Args>(args)...);
    }

    fast_pimpl(fast_pimpl& other) noexcept(noexcept(T(std::declval<const T&>())))
        : fast_pimpl(static_cast<const T&>(other.data_ref())) {}

    fast_pimpl(const fast_pimpl& other) noexcept(noexcept(T(std::declval<const T&>())))
        : fast_pimpl(other.data_ref()) {}

    fast_pimpl(fast_pimpl&& other) noexcept(noexcept(T(std::declval<T>())))
        : fast_pimpl(std::move(other.data_ref())) {}

    fast_pimpl& operator=(const fast_pimpl& other) noexcept(noexcept(std::declval<T&>() = std::declval<const T&>())) {
        if (this == std::addressof(other)) {
            return *this;
        }

        data_ref() = other.data_ref();
        return *this;
    }

    fast_pimpl& operator=(fast_pimpl&& other) noexcept(noexcept(std::declval<T&>() = std::declval<T>())) {
        if (this == std::addressof(other)) {
            return *this;
        }

        data_ref() = std::move(other.data_ref());
        return *this;
    }

    ~fast_pimpl() noexcept {
        static_assert(noexcept(std::declval<T*>()->~T()));
        static_assert(Size != 0);
        static_assert(sizeof(T) == Size);
        static_assert(alignof(T) == Align);

        std::destroy_at(as_type());
    }

    const T& data_ref() const noexcept {
        return *as_type();
    }

    T& data_ref() noexcept {
        return *as_type();
    }

private:
    T* as_type() noexcept {
        return std::launder(reinterpret_cast<T*>(&_storage));
    }

    const T* as_type() const noexcept {
        return std::launder(reinterpret_cast<const T*>(&_storage));
    }

private:
    alignas(Align) std::byte _storage[Size];
};

} // namespace walle::core
