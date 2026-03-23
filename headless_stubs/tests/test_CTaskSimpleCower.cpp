// test_CTaskSimpleCower.cpp — Tests for CTaskSimpleCower.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleCower.h"

GAME_TEST(CTaskSimpleCower, GetTaskType) {
    CTaskSimpleCower task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_COWER);
}

GAME_TEST(CTaskSimpleCower, IsSimple) {
    CTaskSimpleCower task;
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleCower, Clone_PreservesType) {
    CTaskSimpleCower original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_COWER);
    delete cloned;
}

GAME_TEST(CTaskSimpleCower, IsNotInterruptable) {
    CTaskSimpleCower task;
    EXPECT_FALSE(task.IsInterruptable(nullptr));
}
