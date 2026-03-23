// test_CTaskSimpleDrown.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleDrown.h"

GAME_TEST(CTaskSimpleDrown, GetTaskType) {
    CTaskSimpleDrown task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_DROWN);
}

GAME_TEST(CTaskSimpleDrown, IsSimple) {
    CTaskSimpleDrown task;
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleDrown, Clone) {
    CTaskSimpleDrown original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_DROWN);
    delete cloned;
}
