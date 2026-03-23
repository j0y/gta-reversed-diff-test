// test_CTaskComplexInvestigateDisturbance.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskComplexInvestigateDisturbance, GetTaskType) {
    CVector pos(100.0f, 200.0f, 10.0f);
    CTaskComplexInvestigateDisturbance task(pos, nullptr);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_INVESTIGATE_DISTURBANCE);
}

GAME_TEST(CTaskComplexInvestigateDisturbance, IsComplex) {
    CVector pos(0, 0, 0);
    CTaskComplexInvestigateDisturbance task(pos, nullptr);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexInvestigateDisturbance, Constructor_Fields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos(50.0f, 60.0f, 5.0f);
    CTaskComplexInvestigateDisturbance task(pos, player);
    EXPECT_NEAR(task.m_pos.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_pos.y, 60.0f, 1e-5f);
    EXPECT_EQ((uintptr_t)task.m_entity, (uintptr_t)player);
}

GAME_TEST(CTaskComplexInvestigateDisturbance, Clone_PreservesFields) {
    CVector pos(10.0f, 20.0f, 5.0f);
    CTaskComplexInvestigateDisturbance original(pos, nullptr);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_INVESTIGATE_DISTURBANCE);
    auto* c = static_cast<CTaskComplexInvestigateDisturbance*>(cloned);
    EXPECT_NEAR(c->m_pos.x, 10.0f, 1e-5f);
    delete cloned;
}

GAME_TEST(CTaskComplexInvestigateDisturbance, MakeAbortable_NoSubTask) {
    CVector pos(0, 0, 0);
    CTaskComplexInvestigateDisturbance task(pos, nullptr);
    // No subtask => MakeAbortable returns true
    EXPECT_TRUE(task.MakeAbortable(nullptr));
}
