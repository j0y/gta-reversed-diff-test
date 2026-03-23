// test_CTaskComplexBeStill.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexBeStill.h"

GAME_TEST(CTaskComplexBeStill, GetTaskType) {
    CTaskComplexBeStill task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_BE_STILL);
}

GAME_TEST(CTaskComplexBeStill, IsComplex) {
    CTaskComplexBeStill task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexBeStill, Clone) {
    CTaskComplexBeStill original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_BE_STILL);
    delete cloned;
}
