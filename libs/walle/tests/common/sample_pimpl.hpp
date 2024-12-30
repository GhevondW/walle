#pragma once

#include <walle/core/fast_pimpl.hpp>

namespace test::common {

class sample_pimpl {
    struct impl;

public:
    explicit sample_pimpl(const char*);
    sample_pimpl(const sample_pimpl& other);
    sample_pimpl(sample_pimpl&& other) noexcept;
    sample_pimpl& operator=(const sample_pimpl& other);
    sample_pimpl& operator=(sample_pimpl&& other) noexcept;
    ~sample_pimpl();

    [[nodiscard]] const char* foo() const;

private:
    walle::core::fast_pimpl<impl, 32, 8> _impl;
};

} // namespace test::common
