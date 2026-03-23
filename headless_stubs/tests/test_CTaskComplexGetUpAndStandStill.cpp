// test_CTaskComplexGetUpAndStandStill.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexGetUpAndStandStill.h"

GAME_TEST(CTaskComplexGetUpAndStandStill, GetTaskType) {
    CTaskComplexGetUpAndStandStill task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_GET_UP_AND_STAND_STILL);
}

GAME_TEST(CTaskComplexGetUpAndStandStill, IsComplex) {
    CTaskComplexGetUpAndStandStill task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexGetUpAndStandStill, Clone) {
    CTaskComplexGetUpAndStandStill original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_GET_UP_AND_STAND_STILL);
    delete cloned;
}

GAME_TEST(CTaskComplexGetUpAndStandStill, SizeValidation) {
    EXPECT_EQ(sizeof(CTaskComplexGetUpAndStandStill), (size_t)0xC);
}
