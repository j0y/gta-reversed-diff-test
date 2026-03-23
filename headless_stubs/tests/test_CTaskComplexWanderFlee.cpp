// test_CTaskComplexWanderFlee.cpp — Tests for CTaskComplexWanderFlee.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWanderFlee.h"

GAME_TEST(CTaskComplexWanderFlee, GetTaskType) {
    CTaskComplexWanderFlee task(PEDMOVE_SPRINT, 0);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WANDER);
}

GAME_TEST(CTaskComplexWanderFlee, GetWanderType) {
    CTaskComplexWanderFlee task(PEDMOVE_SPRINT, 0);
    EXPECT_EQ((int32)task.GetWanderType(), (int32)WANDER_TYPE_FLEE);
}

GAME_TEST(CTaskComplexWanderFlee, Clone_PreservesFields) {
    CTaskComplexWanderFlee original(PEDMOVE_RUN, 6);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    auto* c = static_cast<CTaskComplexWanderFlee*>(cloned);
    EXPECT_EQ((int32)c->GetWanderType(), (int32)WANDER_TYPE_FLEE);
    EXPECT_EQ((int32)c->m_nMoveState, (int32)PEDMOVE_RUN);
    delete cloned;
}

GAME_TEST(CTaskComplexWanderFlee, IsComplex) {
    CTaskComplexWanderFlee task(PEDMOVE_SPRINT, 0);
    EXPECT_TRUE(task.IsComplex());
}
