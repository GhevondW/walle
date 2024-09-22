#pragma once

#include <cassert>
#include <walle/sync/semaphore.hpp>

#include <cstddef>

namespace walle::sync {

template<typename Tag>
class tagged_semaphore {
    struct token {
        token() = default;
        ~token() {
            assert(!is_valid());
        }
        token(const token& other) = delete;
        token(token&& other) = default;
        token& operator=(const token& other) = delete;
        token& operator=(token&& other) = default;

        [[nodiscard]] bool is_valid() const noexcept { return valid; }
        void invalidate() noexcept { 
            assert(!is_valid());
            valid = false; 
        }


    private:
        bool valid = true;
    };

public:
    using tag_t = Tag;
    using toke_t = token;

    explicit tagged_semaphore(std::size_t tokens)
        :_sema(tokens)
    {}

    [[nodiscard]] token acquire();
    void release(toke_t&& token);

private:
    semaphore _sema;
};

template<typename Tag>
tagged_semaphore<Tag>::toke_t tagged_semaphore<Tag>::acquire() {
    _sema.acquire();
    return toke_t{};
}

template<typename Tag>
void tagged_semaphore<Tag>::release(toke_t&& token) {
    _sema.release();
    token.invalidate();
}

}
