// test_CTaskSimpleStandStill.cpp — Tests for CTaskSimpleStandStill.
// Hook paths: Tasks/TaskTypes/CTaskSimpleStandStill/<fn>
//
// CTaskSimpleStandStill is one of the most basic tasks — makes a ped idle
// for a given duration. Tests verify construction, cloning, and differential
// behavior of MakeAbortable against the original code.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- Construction & type ---

GAME_TEST(CTaskSimpleStandStill, GetTaskType) {
    CTaskSimpleStandStill task(1000, false, false, 8.0f);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_STAND_STILL);
}

GAME_TEST(CTaskSimpleStandStill, IsSimple) {
    CTaskSimpleStandStill task;
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleStandStill, GetSubTask_ReturnsNull) {
    CTaskSimpleStandStill task;
    EXPECT_EQ((uintptr_t)task.GetSubTask(), (uintptr_t)nullptr);
}

GAME_TEST(CTaskSimpleStandStill, DefaultConstructor) {
    CTaskSimpleStandStill task;
    EXPECT_EQ(task.m_nTime, 0);
    EXPECT_FALSE(task.m_bLooped);
    EXPECT_FALSE(task.m_bUseAnimIdleStance);
    EXPECT_NEAR(task.m_fBlendData, 8.0f, 1e-5f);
}

GAME_TEST(CTaskSimpleStandStill, ParameterizedConstructor) {
    CTaskSimpleStandStill task(5000, true, true, 4.0f);
    EXPECT_EQ(task.m_nTime, 5000);
    EXPECT_TRUE(task.m_bLooped);
    EXPECT_TRUE(task.m_bUseAnimIdleStance);
    EXPECT_NEAR(task.m_fBlendData, 4.0f, 1e-5f);
}

// --- Clone ---

GAME_TEST(CTaskSimpleStandStill, Clone_PreservesFields) {
    CTaskSimpleStandStill original(3000, true, false, 2.0f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_STAND_STILL);
    EXPECT_TRUE(cloned->IsSimple());

    auto* c = static_cast<CTaskSimpleStandStill*>(cloned);
    EXPECT_EQ(c->m_nTime, 3000);
    EXPECT_TRUE(c->m_bLooped);
    EXPECT_FALSE(c->m_bUseAnimIdleStance);
    EXPECT_NEAR(c->m_fBlendData, 2.0f, 1e-5f);

    delete cloned;
}

GAME_TEST(CTaskSimpleStandStill, Clone_IsSeparateObject) {
    CTaskSimpleStandStill original(1000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}

// --- Differential: MakeAbortable ---

GAME_DIFF_TEST(CTaskSimpleStandStill, MakeAbortable_Urgent) {
    CTaskSimpleStandStill task(5000);
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleStandStill/MakeAbortable");
      orig = task.MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr); }
    // Recreate task since MakeAbortable may modify state
    CTaskSimpleStandStill task2(5000);
    rev = task2.MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTaskSimpleStandStill, MakeAbortable_Immediate) {
    CTaskSimpleStandStill task(5000);
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleStandStill/MakeAbortable");
      orig = task.MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr); }
    CTaskSimpleStandStill task2(5000);
    rev = task2.MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTaskSimpleStandStill, MakeAbortable_Leisure) {
    CTaskSimpleStandStill task(5000);
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleStandStill/MakeAbortable");
      orig = task.MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr); }
    CTaskSimpleStandStill task2(5000);
    rev = task2.MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    EXPECT_EQ(orig, rev);
}
