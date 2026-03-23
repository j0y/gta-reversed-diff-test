// test_CTaskComplexStuckInAir.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexStuckInAir.h"

GAME_TEST(CTaskComplexStuckInAir, GetTaskType) {
    CTaskComplexStuckInAir task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_STUCK_IN_AIR);
}

GAME_TEST(CTaskComplexStuckInAir, IsComplex) {
    CTaskComplexStuckInAir task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexStuckInAir, Clone) {
    CTaskComplexStuckInAir original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_STUCK_IN_AIR);
    delete cloned;
}

GAME_TEST(CTaskComplexStuckInAir, SizeValidation) {
    EXPECT_EQ(sizeof(CTaskComplexStuckInAir), (size_t)0xC);
}
