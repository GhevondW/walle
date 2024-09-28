#include "sample_pimpl.hpp"

#include <string>
#include <utility>

namespace test::common {

struct sample_pimpl::impl {
    explicit impl(const char* msg)
        : _msg(msg) {}

    std::string _msg;
};

sample_pimpl::sample_pimpl(const char* msg)
    : _impl(msg) {}

sample_pimpl::sample_pimpl(const sample_pimpl& other) = default;
sample_pimpl::sample_pimpl(sample_pimpl&& other) noexcept = default;
sample_pimpl& sample_pimpl::operator=(const sample_pimpl& other) = default;
sample_pimpl& sample_pimpl::operator=(sample_pimpl&& other) noexcept = default;

sample_pimpl::~sample_pimpl() = default;

[[nodiscard]] const char* sample_pimpl::foo() const {
    return _impl.data_ref()._msg.data();
}

} // namespace test::common
