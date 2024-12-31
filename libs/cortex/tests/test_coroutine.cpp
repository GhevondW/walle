#include <cortex/coroutine.hpp>
#include <gtest/gtest.h>

#include <iostream>

//////////////////////////////////////////////////////////////////////

struct TreeNode;

using TreeNodePtr = std::shared_ptr<TreeNode>;

struct TreeNode {
    TreeNodePtr left;
    TreeNodePtr right;
    std::string data;

    TreeNode(std::string d, TreeNodePtr l, TreeNodePtr r)
        : left(std::move(l))
        , right(std::move(r))
        , data(std::move(d)) {}

    static TreeNodePtr Branch(std::string data, TreeNodePtr left, TreeNodePtr right) {
        return std::make_shared<TreeNode>(std::move(data), std::move(left), std::move(right));
    }

    static TreeNodePtr Leaf(std::string data) {
        return std::make_shared<TreeNode>(std::move(data), nullptr, nullptr);
    }
};

//////////////////////////////////////////////////////////////////////

class TreeIterator {
public:
    explicit TreeIterator(TreeNodePtr root)
        : walker_(walle::cortex::coroutine::create([this, root](auto& self) { TreeWalk(root, self); })) {}

    bool TryNext() {
        walker_.resume();
        return !walker_.is_done();
    }

    std::string_view Data() const {
        return data_;
    }

private:
    void TreeWalk(TreeNodePtr node, auto& ctx) {
        if (node->left) {
            TreeWalk(node->left, ctx);
        }

        data_ = node->data;
        ctx.suspend();

        if (node->right) {
            TreeWalk(node->right, ctx);
        }
    }

private:
    walle::cortex::coroutine walker_;
    std::string_view data_;
};

//////////////////////////////////////////////////////////////////////

TEST(cortex_test_coroutine, just_works) {
    int global = 1;

    auto handle = walle::cortex::coroutine::create([&global](auto& ctx) {
        std::cout << "coro -> #1" << std::endl;
        EXPECT_EQ(global++, 1);
        ctx.suspend();
        std::cout << "coro -> #3" << std::endl;
        EXPECT_EQ(global++, 3);
        ctx.suspend();
        std::cout << "coro -> #5" << std::endl;
        EXPECT_EQ(global++, 5);
    });

    handle.resume();
    std::cout << "main -> #2" << std::endl;
    EXPECT_EQ(global++, 2);
    EXPECT_FALSE(handle.is_done());
    handle.resume();
    std::cout << "main -> #4" << std::endl;
    EXPECT_EQ(global++, 4);
    EXPECT_FALSE(handle.is_done());
    handle.resume();
    std::cout << "main -> #6" << std::endl;
    EXPECT_EQ(global++, 6);
    EXPECT_TRUE(handle.is_done());
}

TEST(cortex_test_coroutine, resume_on_finished_coroutine) {
    int global = 0;

    auto handle = walle::cortex::coroutine::create([&global](auto& ctx) {
        std::cout << "coro -> #1" << std::endl;
        EXPECT_EQ(global, 1);
        ++global;
    });

    EXPECT_EQ(global, 0);
    ++global;
    handle.resume();

    EXPECT_EQ(global, 2);
    EXPECT_THROW(handle.resume(), walle::cortex::coroutine::resume_on_completed_coroutine_error_t);
}

TEST(cortex_test_coroutine, exceptions) {
    int global = 0;

    struct custom_exception : std::runtime_error {
        using runtime_error::runtime_error;
    };

    auto handle = walle::cortex::coroutine::create([&global](auto& ctx) {
        std::cout << "coro -> #1" << std::endl;
        EXPECT_EQ(++global, 1);
        ctx.suspend();
        throw custom_exception {"my exception"};
    });

    handle.resume();
    std::cout << "main -> #2" << std::endl;
    EXPECT_EQ(++global, 2);
    EXPECT_FALSE(handle.is_done());

    try {
        handle.resume();
    } catch (const custom_exception& e) {
        ++global;
        std::cout << "rethrowed exception" << std::endl;
    }

    EXPECT_TRUE(handle.is_done());
    EXPECT_EQ(global, 3);
}

TEST(cortex_test_coroutine, destroy_unfinished_coroutine) {
    int global = 0;

    struct test_struct {
        test_struct(int& g)
            : gl(g) {}
        ~test_struct() {
            std::cout << "dtor" << std::endl;
            ++gl;
        }
        int& gl;
    };

    {
        auto handle = walle::cortex::coroutine::create([&global](auto& ctx) {
            std::cout << "coro -> #2" << std::endl;
            test_struct ts(global);
            EXPECT_EQ(global, 1);
            ++global;
            ctx.suspend();

            EXPECT_FALSE(true); // we must not get here
        });

        std::cout << "main -> #1" << std::endl;
        EXPECT_EQ(global, 0);
        ++global;
        handle.resume();

        std::cout << "main -> #3" << std::endl;
        EXPECT_EQ(global, 2);
    }
    std::cout << "main -> #4" << std::endl;
    EXPECT_EQ(global, 3);
}

TEST(cortex_test_coroutine, nested) {
    int counter = 0;

    auto execution_one = walle::cortex::coroutine::create([&counter](auto& suspender) {
        EXPECT_EQ(++counter, 2);
        std::cout << "Step 2" << '\n';
        suspender.suspend();

        EXPECT_EQ(++counter, 4);
        std::cout << "Step 4" << '\n';
    });

    auto execution_two = walle::cortex::coroutine::create([&execution_one, &counter](auto& suspender) {
        EXPECT_EQ(++counter, 1);
        std::cout << "Step 1" << '\n';
        execution_one.resume();

        EXPECT_EQ(++counter, 3);
        std::cout << "Step 3" << '\n';
        execution_one.resume();

        auto nested = walle::cortex::coroutine::create([&counter](auto& suspender) {
            EXPECT_EQ(++counter, 5);
            std::cout << "Step 5" << '\n';
            suspender.suspend();

            EXPECT_EQ(++counter, 7);
            std::cout << "Step 7" << '\n';
        });

        nested.resume();

        EXPECT_EQ(++counter, 6);
        std::cout << "Step 6" << '\n';
        nested.resume();
    });

    execution_two.resume();

    EXPECT_EQ(counter, 7);
}

TEST(cortex_test_coroutine, suspend) {
    int step = 0;

    auto coro = walle::cortex::coroutine::create([&step](auto& self) {
        ++step;
        self.suspend();
        ++step;
    });

    ASSERT_EQ(step, 0);
    ASSERT_FALSE(coro.is_done());

    coro.resume();

    ASSERT_EQ(step, 1);
    ASSERT_FALSE(coro.is_done());

    coro.resume();

    ASSERT_EQ(step, 2);
    ASSERT_TRUE(coro.is_done());
}

TEST(cortex_test_coroutine, suspend_for_loop) {
    const size_t kIters = 128;

    auto coro = walle::cortex::coroutine::create([](auto& self) {
        for (size_t i = 0; i < kIters; ++i) {
            self.suspend();
        }
    });

    for (size_t i = 0; i < kIters; ++i) {
        coro.resume();
    }

    ASSERT_FALSE(coro.is_done());

    coro.resume(); // Last step

    ASSERT_TRUE(coro.is_done());
}

TEST(cortex_test_coroutine, interleaving) {
    int step = 0;

    auto a = walle::cortex::coroutine::create([&step](auto& self) {
        ASSERT_EQ(step, 0);
        step = 1;
        self.suspend();
        ASSERT_EQ(step, 2);
        step = 3;
    });

    auto b = walle::cortex::coroutine::create([&step](auto& self) {
        ASSERT_EQ(step, 1);
        step = 2;
        self.suspend();
        ASSERT_EQ(step, 3);
        step = 4;
    });

    a.resume();
    b.resume();

    ASSERT_EQ(step, 2);

    a.resume();
    b.resume();

    ASSERT_TRUE(a.is_done());
    ASSERT_TRUE(b.is_done());

    ASSERT_EQ(step, 4);
}

struct Threads {
    template <typename F>
    void Run(F task) {
        std::thread t([task = std::move(task)]() mutable { task(); });
        t.join();
    }
};

TEST(cortex_test_coroutine, threads) {
    size_t steps = 0;

    auto coro = walle::cortex::coroutine::create([&steps](auto& self) {
        ++steps;
        self.suspend();
        ++steps;
        self.suspend();
        ++steps;
    });

    auto step = [&coro]() { coro.resume(); };

    // Simulate fiber running on thread pool
    Threads threads;

    threads.Run(step);
    ASSERT_EQ(steps, 1);

    threads.Run(step);
    ASSERT_EQ(steps, 2);

    threads.Run(step);
    ASSERT_EQ(steps, 3);
}

TEST(cortex_test_coroutine, tree_walk) {
    auto tree = TreeNode::Branch(
        "B",
        TreeNode::Leaf("A"),
        TreeNode::Branch("F", TreeNode::Branch("D", TreeNode::Leaf("C"), TreeNode::Leaf("E")), TreeNode::Leaf("G")));

    std::stringstream traversal;

    TreeIterator iter(tree);
    while (iter.TryNext()) {
        traversal << iter.Data();
    }

    ASSERT_EQ(traversal.str(), "ABCDEFG");
}

TEST(cortex_test_coroutine, pipeline) {
    const size_t kSteps = 123;

    size_t steps = 0;

    auto outer = walle::cortex::coroutine::create([&](auto& self) {
        auto inner = walle::cortex::coroutine::create([&steps](auto& self) {
            for (size_t i = 0; i < kSteps; ++i) {
                ++steps;
                self.suspend();
            }
        });

        while (!inner.is_done()) {
            inner.resume();
            self.suspend();
        }
    });

    while (!outer.is_done()) {
        outer.resume();
    }

    ASSERT_EQ(steps, kSteps);
}
