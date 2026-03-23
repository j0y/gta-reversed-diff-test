// test_CTaskComplexWanderCriminal.cpp — Tests for CTaskComplexWanderCriminal.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWanderCriminal.h"

GAME_TEST(CTaskComplexWanderCriminal, GetTaskType) {
    CTaskComplexWanderCriminal task(PEDMOVE_WALK, 0);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WANDER);
}

GAME_TEST(CTaskComplexWanderCriminal, GetWanderType) {
    CTaskComplexWanderCriminal task(PEDMOVE_WALK, 0);
    EXPECT_EQ((int32)task.GetWanderType(), (int32)WANDER_TYPE_CRIMINAL);
}

GAME_TEST(CTaskComplexWanderCriminal, Clone_PreservesFields) {
    CTaskComplexWanderCriminal original(PEDMOVE_SPRINT, 5);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    auto* c = static_cast<CTaskComplexWanderCriminal*>(cloned);
    EXPECT_EQ((int32)c->GetWanderType(), (int32)WANDER_TYPE_CRIMINAL);
    EXPECT_EQ((int32)c->m_nMoveState, (int32)PEDMOVE_SPRINT);
    EXPECT_EQ(c->m_nDir, (uint8)5);
    delete cloned;
}

GAME_TEST(CTaskComplexWanderCriminal, IsComplex) {
    CTaskComplexWanderCriminal task(PEDMOVE_WALK, 0);
    EXPECT_TRUE(task.IsComplex());
}
