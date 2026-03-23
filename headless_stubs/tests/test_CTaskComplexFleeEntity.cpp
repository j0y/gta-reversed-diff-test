// test_CTaskComplexFleeEntity.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexFleeEntity.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskComplexFleeEntity, GetTaskType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexFleeEntity task(player, true, 30.0f);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_FLEE_ENTITY);
}

GAME_TEST(CTaskComplexFleeEntity, IsComplex) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexFleeEntity task(player, false, 20.0f);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexFleeEntity, Constructor_Fields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexFleeEntity task(player, true, 50.0f, 8000, 2000, 3.0f);
    EXPECT_EQ((uintptr_t)task.m_entity, (uintptr_t)player);
    EXPECT_TRUE(task.m_pedScream);
    EXPECT_NEAR(task.m_safeDistance, 50.0f, 1e-5f);
    EXPECT_EQ(task.m_time, 8000);
    EXPECT_EQ(task.m_posCheckPeriod, 2000);
    EXPECT_NEAR(task.m_posChangeTolerance, 3.0f, 1e-5f);
}

GAME_TEST(CTaskComplexFleeEntity, Constructor_Defaults) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexFleeEntity task(player, false, 60.0f);
    EXPECT_EQ(task.m_time, 1000000);
    EXPECT_EQ(task.m_posCheckPeriod, 1000);
    EXPECT_NEAR(task.m_posChangeTolerance, 1.0f, 1e-5f);
}

GAME_TEST(CTaskComplexFleeEntity, Clone_PreservesFields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexFleeEntity original(player, true, 40.0f, 5000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_FLEE_ENTITY);
    auto* c = static_cast<CTaskComplexFleeEntity*>(cloned);
    EXPECT_TRUE(c->m_pedScream);
    EXPECT_NEAR(c->m_safeDistance, 40.0f, 1e-5f);
    delete cloned;
}
