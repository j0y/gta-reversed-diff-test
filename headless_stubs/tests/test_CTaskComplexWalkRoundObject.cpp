// test_CTaskComplexWalkRoundObject.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundObject.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskComplexWalkRoundObject, GetTaskType) {
    CVector target(100.0f, 200.0f, 10.0f);
    CTaskComplexWalkRoundObject task(PEDMOVE_WALK, target, nullptr);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WALK_ROUND_OBJECT);
}

GAME_TEST(CTaskComplexWalkRoundObject, IsComplex) {
    CVector target(0, 0, 0);
    CTaskComplexWalkRoundObject task(PEDMOVE_RUN, target, nullptr);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexWalkRoundObject, Constructor_Fields) {
    CVector target(50.0f, 75.0f, 12.0f);
    CTaskComplexWalkRoundObject task(PEDMOVE_SPRINT, target, nullptr);
    EXPECT_EQ(task.m_moveState, (int32)PEDMOVE_SPRINT);
    EXPECT_NEAR(task.m_targetPoint.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_targetPoint.y, 75.0f, 1e-5f);
    EXPECT_EQ((uintptr_t)task.m_object, (uintptr_t)nullptr);
}

GAME_TEST(CTaskComplexWalkRoundObject, Clone_PreservesFields) {
    CVector target(10.0f, 20.0f, 5.0f);
    CTaskComplexWalkRoundObject original(PEDMOVE_WALK, target, nullptr);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_WALK_ROUND_OBJECT);
    auto* c = static_cast<CTaskComplexWalkRoundObject*>(cloned);
    EXPECT_NEAR(c->m_targetPoint.x, 10.0f, 1e-5f);
    delete cloned;
}
