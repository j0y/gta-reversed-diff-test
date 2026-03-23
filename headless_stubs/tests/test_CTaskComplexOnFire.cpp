// test_CTaskComplexOnFire.cpp — Tests for CTaskComplexOnFire.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexOnFire.h"

GAME_TEST(CTaskComplexOnFire, GetTaskType) {
    CTaskComplexOnFire task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_ON_FIRE);
}

GAME_TEST(CTaskComplexOnFire, IsComplex) {
    CTaskComplexOnFire task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexOnFire, Clone_PreservesType) {
    CTaskComplexOnFire original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_ON_FIRE);
    delete cloned;
}
