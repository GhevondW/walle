#include <gtest/gtest.h>
#include <walle/exec/executor.hpp>
#include <walle/exec/manual_executor.hpp>

// namespace walle::core {

class ManualExecutorTest : public ::testing::Test {
protected:
    walle::exec::manual_executor executor;
};

TEST_F(ManualExecutorTest, TasksCountInitiallyZero) {
    executor.start();
    EXPECT_EQ(executor.tasks_count(), 0u);
}

TEST_F(ManualExecutorTest, SubmitIncreasesTaskCount) {
    executor.start();

    executor.submit([] {});
    EXPECT_EQ(executor.tasks_count(), 1u);

    executor.submit([] {});
    EXPECT_EQ(executor.tasks_count(), 2u);
}

TEST_F(ManualExecutorTest, RunOneExecutesOneTask) {
    executor.start();

    bool task_executed = false;
    executor.submit([&task_executed] { task_executed = true; });

    executor.run_one();
    EXPECT_TRUE(task_executed);
    EXPECT_EQ(executor.tasks_count(), 0u);
}

TEST_F(ManualExecutorTest, RunOneThrowsEmptyExecutorWhenNoTasks) {
    executor.start();
    EXPECT_THROW(executor.run_one(), walle::exec::manual_executor::empty_executor);
}

TEST_F(ManualExecutorTest, RunAllExecutesAllTasks) {
    executor.start();
    int task_count = 0;
    executor.submit([&task_count] { task_count++; });
    executor.submit([&task_count] { task_count++; });

    executor.run_all();
    EXPECT_EQ(task_count, 2);
    EXPECT_EQ(executor.tasks_count(), 0u);
}

TEST_F(ManualExecutorTest, RunAllDoesNothingIfEmpty) {
    executor.start();
    EXPECT_NO_THROW(executor.run_all());
    EXPECT_EQ(executor.tasks_count(), 0u);
}

TEST_F(ManualExecutorTest, SubmitThrowsOnEmptyTask) {
    executor.start();
    EXPECT_THROW(executor.submit(nullptr), walle::exec::manual_executor::empty_task);
}

TEST_F(ManualExecutorTest, RunAllWithMixedTaskExecution) {
    executor.start();
    bool task1_executed = false;
    bool task2_executed = false;

    executor.submit([&task1_executed] { task1_executed = true; });
    executor.submit([&task2_executed] { task2_executed = true; });

    executor.run_all();
    EXPECT_TRUE(task1_executed);
    EXPECT_TRUE(task2_executed);
    EXPECT_EQ(executor.tasks_count(), 0u);
}

TEST_F(ManualExecutorTest, DestructorDoesNotThrow) {
    executor.start();
    EXPECT_NO_THROW({ walle::exec::manual_executor local_executor; });
}

// } // namespace walle::core
