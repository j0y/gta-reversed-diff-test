// test_CTaskComplexInAirAndLand.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexInAirAndLand.h"

GAME_TEST(CTaskComplexInAirAndLand, GetTaskType) {
    CTaskComplexInAirAndLand task(false, false);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_IN_AIR_AND_LAND);
}

GAME_TEST(CTaskComplexInAirAndLand, IsComplex) {
    CTaskComplexInAirAndLand task(false, false);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexInAirAndLand, Constructor_JumpGlide) {
    CTaskComplexInAirAndLand task(true, false);
    EXPECT_TRUE(task.m_bUsingJumpGlide);
    EXPECT_FALSE(task.m_bUsingFallGlide);
}

GAME_TEST(CTaskComplexInAirAndLand, Constructor_FallGlide) {
    CTaskComplexInAirAndLand task(false, true);
    EXPECT_FALSE(task.m_bUsingJumpGlide);
    EXPECT_TRUE(task.m_bUsingFallGlide);
}

GAME_TEST(CTaskComplexInAirAndLand, Clone_PreservesFlags) {
    CTaskComplexInAirAndLand original(true, true);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_IN_AIR_AND_LAND);
    auto* c = static_cast<CTaskComplexInAirAndLand*>(cloned);
    EXPECT_TRUE(c->m_bUsingJumpGlide);
    EXPECT_TRUE(c->m_bUsingFallGlide);
    delete cloned;
}

GAME_TEST(CTaskComplexInAirAndLand, SizeValidation) {
    EXPECT_EQ(sizeof(CTaskComplexInAirAndLand), (size_t)0x10);
}
