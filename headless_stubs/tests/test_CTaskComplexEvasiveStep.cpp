// test_CTaskComplexEvasiveStep.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexEvasiveStep.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskComplexEvasiveStep, GetTaskType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos(100.0f, 200.0f, 10.0f);
    CTaskComplexEvasiveStep task(player, pos);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_EVASIVE_STEP);
}

GAME_TEST(CTaskComplexEvasiveStep, IsComplex) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos(0, 0, 0);
    CTaskComplexEvasiveStep task(player, pos);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexEvasiveStep, Constructor_Fields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos(50.0f, 75.0f, 12.0f);
    CTaskComplexEvasiveStep task(player, pos);
    EXPECT_EQ((uintptr_t)task.m_Entity, (uintptr_t)player);
    EXPECT_NEAR(task.m_Pos.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_Pos.y, 75.0f, 1e-5f);
}

GAME_TEST(CTaskComplexEvasiveStep, Clone_PreservesFields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos(10.0f, 20.0f, 5.0f);
    CTaskComplexEvasiveStep original(player, pos);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_EVASIVE_STEP);
    auto* c = static_cast<CTaskComplexEvasiveStep*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_Entity, (uintptr_t)player);
    EXPECT_NEAR(c->m_Pos.x, 10.0f, 1e-5f);
    delete cloned;
}
