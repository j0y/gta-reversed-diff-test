// test_CTaskComplexSmartFleeEntity.cpp — Tests for CTaskComplexSmartFleeEntity.
// Hook paths: Tasks/TaskTypes/CTaskComplexSmartFleeEntity/<fn>
//
// AI flee task — makes a ped flee from an entity. Tests verify construction,
// cloning, and parameter handling.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleeEntity.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskComplexSmartFleeEntity, GetTaskType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexSmartFleeEntity task(player, true, 30.0f);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_SMART_FLEE_ENTITY);
}

GAME_TEST(CTaskComplexSmartFleeEntity, IsComplex) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexSmartFleeEntity task(player, false, 20.0f);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexSmartFleeEntity, Constructor_Fields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexSmartFleeEntity task(player, true, 50.0f, 5000, 2000, 2.0f);
    EXPECT_EQ((uintptr_t)task.m_fleeFrom, (uintptr_t)player);
    EXPECT_TRUE(task.m_pedScream);
    EXPECT_NEAR(task.m_safeDistance, 50.0f, 1e-5f);
    EXPECT_EQ(task.m_time, 5000);
    EXPECT_EQ(task.m_posCheckPeriod, 2000);
    EXPECT_NEAR(task.m_posChangeTolerance, 2.0f, 1e-5f);
}

GAME_TEST(CTaskComplexSmartFleeEntity, Constructor_Defaults) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexSmartFleeEntity task(player, false, 25.0f);
    EXPECT_EQ(task.m_time, 1000000);
    EXPECT_EQ(task.m_posCheckPeriod, 1000);
    EXPECT_NEAR(task.m_posChangeTolerance, 1.0f, 1e-5f);
    EXPECT_EQ((int32)task.m_moveState, (int32)PEDMOVE_SPRINT);
}

GAME_TEST(CTaskComplexSmartFleeEntity, Clone_PreservesFields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexSmartFleeEntity original(player, true, 40.0f, 8000, 500, 3.0f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_SMART_FLEE_ENTITY);
    auto* c = static_cast<CTaskComplexSmartFleeEntity*>(cloned);
    EXPECT_TRUE(c->m_pedScream);
    EXPECT_NEAR(c->m_safeDistance, 40.0f, 1e-5f);
    EXPECT_EQ(c->m_time, 8000);

    delete cloned;
}
