// test_CTaskComplexWanderMedic.cpp — Tests for CTaskComplexWanderMedic.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWanderMedic.h"

GAME_TEST(CTaskComplexWanderMedic, GetTaskType) {
    CTaskComplexWanderMedic task(PEDMOVE_WALK, 0);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WANDER);
}

GAME_TEST(CTaskComplexWanderMedic, GetWanderType) {
    CTaskComplexWanderMedic task(PEDMOVE_WALK, 0);
    EXPECT_EQ((int32)task.GetWanderType(), (int32)WANDER_TYPE_MEDIC);
}

GAME_TEST(CTaskComplexWanderMedic, Clone_PreservesType) {
    CTaskComplexWanderMedic original(PEDMOVE_RUN, 3, true);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_WANDER);
    auto* c = static_cast<CTaskComplexWanderMedic*>(cloned);
    EXPECT_EQ((int32)c->GetWanderType(), (int32)WANDER_TYPE_MEDIC);
    EXPECT_EQ((int32)c->m_nMoveState, (int32)PEDMOVE_RUN);
    EXPECT_EQ(c->m_nDir, (uint8)3);
    delete cloned;
}

GAME_TEST(CTaskComplexWanderMedic, IsComplex) {
    CTaskComplexWanderMedic task(PEDMOVE_WALK, 0);
    EXPECT_TRUE(task.IsComplex());
}
