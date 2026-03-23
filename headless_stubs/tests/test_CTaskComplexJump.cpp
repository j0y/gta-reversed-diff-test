// test_CTaskComplexJump.cpp — Tests for CTaskComplexJump.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexJump.h"

GAME_TEST(CTaskComplexJump, GetTaskType) {
    CTaskComplexJump task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_JUMP);
}

GAME_TEST(CTaskComplexJump, IsComplex) {
    CTaskComplexJump task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexJump, Constructor_Default) {
    CTaskComplexJump task;
    EXPECT_EQ((int32)task.m_ForceClimb, (int32)CTaskComplexJump::eForceClimb::OK);
}

GAME_TEST(CTaskComplexJump, Constructor_ForceClimb) {
    CTaskComplexJump task(CTaskComplexJump::eForceClimb::FORCE);
    EXPECT_EQ((int32)task.m_ForceClimb, (int32)CTaskComplexJump::eForceClimb::FORCE);
}

GAME_TEST(CTaskComplexJump, Constructor_DisableClimb) {
    CTaskComplexJump task(CTaskComplexJump::eForceClimb::DISABLE);
    EXPECT_EQ((int32)task.m_ForceClimb, (int32)CTaskComplexJump::eForceClimb::DISABLE);
}

GAME_TEST(CTaskComplexJump, Clone_PreservesType) {
    CTaskComplexJump original(CTaskComplexJump::eForceClimb::FORCE);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_JUMP);
    delete cloned;
}
