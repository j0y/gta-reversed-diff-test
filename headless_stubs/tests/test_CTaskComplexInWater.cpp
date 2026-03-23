// test_CTaskComplexInWater.cpp — Tests for CTaskComplexInWater.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexInWater.h"

GAME_TEST(CTaskComplexInWater, GetTaskType) {
    CTaskComplexInWater task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_IN_WATER);
}

GAME_TEST(CTaskComplexInWater, IsComplex) {
    CTaskComplexInWater task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexInWater, Clone_PreservesType) {
    CTaskComplexInWater original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_IN_WATER);
    delete cloned;
}
