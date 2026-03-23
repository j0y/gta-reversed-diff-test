// test_CTaskComplexUseGoggles.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexUseGoggles.h"

GAME_TEST(CTaskComplexUseGoggles, GetTaskType) {
    CTaskComplexUseGoggles task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_USE_GOGGLES);
}

GAME_TEST(CTaskComplexUseGoggles, IsComplex) {
    CTaskComplexUseGoggles task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexUseGoggles, Clone) {
    CTaskComplexUseGoggles original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_USE_GOGGLES);
    delete cloned;
}
