#pragma once

#include <cstddef>
#include <memory>
#include <new>
#include <utility>

/// @namespace walle::core
/// @brief Core namespace for fundamental utilities.
namespace walle::core {

/**
 * @class fast_pimpl
 * @brief A fast PIMPL (Pointer to Implementation) utility.
 *
 * This template provides an efficient way to hide implementation details
 * while maintaining alignment and size constraints.
 *
 * @tparam T The type of the implementation.
 * @tparam Size The size of the storage.
 * @tparam Align The alignment of the storage.
 */
template <typename T, std::size_t Size, std::size_t Align>
class fast_pimpl final {
public:
    /**
     * @brief Constructs a fast_pimpl with forwarded arguments.
     * @param args Arguments forwarded to the constructor of T.
     */
    template <typename... Args>
    explicit fast_pimpl(Args&&... args) noexcept(noexcept(T(std::declval<Args>()...)))
        : _storage() {
        new (as_type()) T(std::forward<Args>(args)...);
    }

    /**
     * @brief Copy constructor.
     * @param other The object to copy from.
     */
    fast_pimpl(fast_pimpl& other) noexcept(noexcept(T(std::declval<const T&>())))
        : fast_pimpl(static_cast<const T&>(other.data_ref())) {}

    /**
     * @brief Copy constructor.
     * @param other The object to copy from.
     */
    fast_pimpl(const fast_pimpl& other) noexcept(noexcept(T(std::declval<const T&>())))
        : fast_pimpl(other.data_ref()) {}

    /**
     * @brief Move constructor.
     * @param other The object to move from.
     */
    fast_pimpl(fast_pimpl&& other) noexcept(noexcept(T(std::declval<T>())))
        : fast_pimpl(std::move(other.data_ref())) {}

    /**
     * @brief Copy assignment operator.
     * @param other The object to copy from.
     * @return Reference to this object.
     */
    fast_pimpl& operator=(const fast_pimpl& other) noexcept(noexcept(std::declval<T&>() = std::declval<const T&>())) {
        if (this == std::addressof(other)) {
            return *this;
        }

        data_ref() = other.data_ref();
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The object to move from.
     * @return Reference to this object.
     */
    fast_pimpl& operator=(fast_pimpl&& other) noexcept(noexcept(std::declval<T&>() = std::declval<T>())) {
        if (this == std::addressof(other)) {
            return *this;
        }

        data_ref() = std::move(other.data_ref());
        return *this;
    }

    ~fast_pimpl() {
        static_assert(noexcept(std::declval<T*>()->~T()));
        static_assert(Size != 0);
        static_assert(sizeof(T) == Size);
        static_assert(alignof(T) == Align);

        std::destroy_at(as_type());
    }

    /**
     * @brief Provides const access to the stored object.
     * @return Const reference to the stored object.
     */
    const T& data_ref() const noexcept {
        return *as_type();
    }

    /**
     * @brief Provides mutable access to the stored object.
     * @return Reference to the stored object.
     */
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
    alignas(Align) std::byte _storage[Size]; ///< Internal storage for the implementation.
};

} // namespace walle::core
