// test_CTaskComplexEvasiveCower.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexEvasiveCower.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskComplexEvasiveCower, GetTaskType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos(0, 0, 0);
    CTaskComplexEvasiveCower task(player, pos);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_EVASIVE_COWER);
}

GAME_TEST(CTaskComplexEvasiveCower, IsComplex) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos(0, 0, 0);
    CTaskComplexEvasiveCower task(player, pos);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexEvasiveCower, Clone_PreservesFields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos(30.0f, 40.0f, 5.0f);
    CTaskComplexEvasiveCower original(player, pos);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_EVASIVE_COWER);
    auto* c = static_cast<CTaskComplexEvasiveCower*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_Entity, (uintptr_t)player);
    EXPECT_NEAR(c->m_Pos.x, 30.0f, 1e-5f);
    delete cloned;
}
