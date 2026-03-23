// test_CTaskComplexWanderCop.cpp — Tests for CTaskComplexWanderCop.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWanderCop.h"

GAME_TEST(CTaskComplexWanderCop, GetTaskType) {
    CTaskComplexWanderCop task(PEDMOVE_WALK, 0);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WANDER);
}

GAME_TEST(CTaskComplexWanderCop, GetWanderType) {
    CTaskComplexWanderCop task(PEDMOVE_WALK, 0);
    EXPECT_EQ((int32)task.GetWanderType(), (int32)WANDER_TYPE_COP);
}

GAME_TEST(CTaskComplexWanderCop, Clone_PreservesFields) {
    CTaskComplexWanderCop original(PEDMOVE_RUN, 2);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    auto* c = static_cast<CTaskComplexWanderCop*>(cloned);
    EXPECT_EQ((int32)c->GetWanderType(), (int32)WANDER_TYPE_COP);
    EXPECT_EQ((int32)c->m_nMoveState, (int32)PEDMOVE_RUN);
    EXPECT_EQ(c->m_nDir, (uint8)2);
    delete cloned;
}

GAME_TEST(CTaskComplexWanderCop, IsComplex) {
    CTaskComplexWanderCop task(PEDMOVE_WALK, 0);
    EXPECT_TRUE(task.IsComplex());
}
