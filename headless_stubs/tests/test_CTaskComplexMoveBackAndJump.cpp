// test_CTaskComplexMoveBackAndJump.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexMoveBackAndJump.h"

GAME_TEST(CTaskComplexMoveBackAndJump, GetTaskType) {
    CTaskComplexMoveBackAndJump task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_MOVE_BACK_AND_JUMP);
}

GAME_TEST(CTaskComplexMoveBackAndJump, IsComplex) {
    CTaskComplexMoveBackAndJump task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexMoveBackAndJump, Clone) {
    CTaskComplexMoveBackAndJump original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_MOVE_BACK_AND_JUMP);
    delete cloned;
}
