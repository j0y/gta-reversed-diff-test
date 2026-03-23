// test_CTaskSimpleChoking.cpp — Tests for CTaskSimpleChoking.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleChoking.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskSimpleChoking, GetTaskType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskSimpleChoking task(player, false);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_CHOKING);
}

GAME_TEST(CTaskSimpleChoking, IsSimple) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskSimpleChoking task(player, false);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleChoking, Constructor_NotTeargas) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskSimpleChoking task(player, false);
    EXPECT_EQ((uintptr_t)task.m_pAttacker, (uintptr_t)player);
    EXPECT_FALSE(task.m_bIsTeargas);
    EXPECT_FALSE(task.m_bIsFinished);
}

GAME_TEST(CTaskSimpleChoking, Constructor_Teargas) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskSimpleChoking task(player, true);
    EXPECT_TRUE(task.m_bIsTeargas);
}

GAME_TEST(CTaskSimpleChoking, Clone_PreservesType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskSimpleChoking original(player, true);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_CHOKING);
    delete cloned;
}
