#pragma once

#include <cassert>
#include <walle/core/semaphore.hpp>

#include <cstddef>

namespace walle::core {

template <typename Tag>
class tagged_semaphore {
    struct token {
        token() = default;
        ~token() = default;
        token(const token& other) = delete;
        token(token&& other) noexcept = default;
        token& operator=(const token& other) = delete;
        token& operator=(token&& other) noexcept = default;

        [[nodiscard]] bool is_valid() const noexcept {
            return valid;
        }

        void invalidate() noexcept {
            assert(is_valid());
            valid = false;
        }

    private:
        bool valid = true;
    };

public:
    using tag_t = Tag;
    using toke_t = token;

    explicit tagged_semaphore(std::size_t tokens)
        : _sema(tokens) {}

    [[nodiscard]] token acquire();
    void release(toke_t&& in_token);

private:
    semaphore _sema;
};

template <typename Tag>
typename tagged_semaphore<Tag>::toke_t tagged_semaphore<Tag>::acquire() {
    _sema.acquire();
    return toke_t {};
}

template <typename Tag>
void tagged_semaphore<Tag>::release(toke_t&& in_token) {
    _sema.release();
    in_token.invalidate();
}

} // namespace walle::sync
