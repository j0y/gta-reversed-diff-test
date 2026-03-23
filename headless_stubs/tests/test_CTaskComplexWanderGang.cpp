// test_CTaskComplexWanderGang.cpp — Tests for CTaskComplexWanderGang.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWanderGang.h"

GAME_TEST(CTaskComplexWanderGang, GetTaskType) {
    CTaskComplexWanderGang task(PEDMOVE_WALK, 0, 5000, true, 0.5f);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WANDER);
}

GAME_TEST(CTaskComplexWanderGang, GetWanderType) {
    CTaskComplexWanderGang task(PEDMOVE_WALK, 0, 5000, true, 0.5f);
    EXPECT_EQ((int32)task.GetWanderType(), (int32)WANDER_TYPE_GANG);
}

GAME_TEST(CTaskComplexWanderGang, Clone_PreservesFields) {
    CTaskComplexWanderGang original(PEDMOVE_RUN, 4, 3000, false, 2.0f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    auto* c = static_cast<CTaskComplexWanderGang*>(cloned);
    EXPECT_EQ((int32)c->GetWanderType(), (int32)WANDER_TYPE_GANG);
    EXPECT_EQ((int32)c->m_nMoveState, (int32)PEDMOVE_RUN);
    EXPECT_EQ(c->m_nDir, (uint8)4);
    delete cloned;
}

GAME_TEST(CTaskComplexWanderGang, IsComplex) {
    CTaskComplexWanderGang task(PEDMOVE_WALK, 0, 5000, true, 0.5f);
    EXPECT_TRUE(task.IsComplex());
}
