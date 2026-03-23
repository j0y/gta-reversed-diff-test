// test_CTaskSimpleInAir.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleInAir.h"

GAME_TEST(CTaskSimpleInAir, GetTaskType) {
    CTaskSimpleInAir task(false, false, false);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_IN_AIR);
}

GAME_TEST(CTaskSimpleInAir, IsSimple) {
    CTaskSimpleInAir task(false, false, false);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleInAir, Constructor_JumpGlide) {
    CTaskSimpleInAir task(true, false, false);
    EXPECT_TRUE(task.m_bUsingJumpGlide);
    EXPECT_FALSE(task.m_bUsingFallGlide);
    EXPECT_FALSE(task.m_bUsingClimbJump);
}

GAME_TEST(CTaskSimpleInAir, Constructor_FallGlide) {
    CTaskSimpleInAir task(false, true, false);
    EXPECT_FALSE(task.m_bUsingJumpGlide);
    EXPECT_TRUE(task.m_bUsingFallGlide);
}

GAME_TEST(CTaskSimpleInAir, Constructor_ClimbJump) {
    CTaskSimpleInAir task(false, false, true);
    EXPECT_TRUE(task.m_bUsingClimbJump);
}

GAME_TEST(CTaskSimpleInAir, Clone_PreservesFlags) {
    CTaskSimpleInAir original(true, true, false);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_IN_AIR);
    auto* c = static_cast<CTaskSimpleInAir*>(cloned);
    EXPECT_TRUE(c->m_bUsingJumpGlide);
    EXPECT_TRUE(c->m_bUsingFallGlide);
    EXPECT_FALSE(c->m_bUsingClimbJump);
    delete cloned;
}
