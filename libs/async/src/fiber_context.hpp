#pragma once

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <walle/exec/executor.hpp>

namespace walle::async {

class fiber_context_i {
public:
    fiber_context_i()
        : _id(boost::uuids::random_generator()()) {}

    virtual ~fiber_context_i() = default;

    boost::uuids::uuid id() const {
        return _id;
    }

    // virtual void resume() = 0;
    virtual void yield() = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;

    virtual exec::executor_i* scheduler() const = 0;

private:
    boost::uuids::uuid _id;
};

} // namespace walle::async
