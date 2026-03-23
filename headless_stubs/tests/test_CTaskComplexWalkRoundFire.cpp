// test_CTaskComplexWalkRoundFire.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundFire.h"

GAME_TEST(CTaskComplexWalkRoundFire, GetTaskType) {
    CVector firePos(100.0f, 200.0f, 10.0f);
    CVector targetPos(120.0f, 210.0f, 10.0f);
    CTaskComplexWalkRoundFire task(PEDMOVE_RUN, firePos, 5.0f, targetPos);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WALK_ROUND_FIRE);
}

GAME_TEST(CTaskComplexWalkRoundFire, IsComplex) {
    CVector v(0, 0, 0);
    CTaskComplexWalkRoundFire task(PEDMOVE_WALK, v, 3.0f, v);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexWalkRoundFire, Constructor_Fields) {
    CVector firePos(50.0f, 60.0f, 5.0f);
    CVector targetPos(70.0f, 80.0f, 5.0f);
    CTaskComplexWalkRoundFire task(PEDMOVE_SPRINT, firePos, 8.0f, targetPos);
    EXPECT_EQ((int32)task.m_moveState, (int32)PEDMOVE_SPRINT);
    EXPECT_NEAR(task.m_fireBoundSphere.m_vecCenter.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_fireBoundSphere.m_fRadius, 8.0f, 1e-5f);
    EXPECT_NEAR(task.m_targetPos.x, 70.0f, 1e-5f);
}

GAME_TEST(CTaskComplexWalkRoundFire, Clone_PreservesFields) {
    CVector firePos(10.0f, 20.0f, 5.0f);
    CVector targetPos(30.0f, 40.0f, 5.0f);
    CTaskComplexWalkRoundFire original(PEDMOVE_RUN, firePos, 4.0f, targetPos);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_WALK_ROUND_FIRE);
    auto* c = static_cast<CTaskComplexWalkRoundFire*>(cloned);
    EXPECT_NEAR(c->m_fireBoundSphere.m_fRadius, 4.0f, 1e-5f);
    EXPECT_NEAR(c->m_targetPos.x, 30.0f, 1e-5f);
    delete cloned;
}

GAME_TEST(CTaskComplexWalkRoundFire, CreateNextSubTask_ReturnsNull) {
    CVector v(0, 0, 0);
    CTaskComplexWalkRoundFire task(PEDMOVE_WALK, v, 1.0f, v);
    EXPECT_EQ((uintptr_t)task.CreateNextSubTask(nullptr), (uintptr_t)nullptr);
}
