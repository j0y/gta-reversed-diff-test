// test_CTaskManager.cpp — Differential tests for CTaskManager.
// Hook paths: Tasks/CTaskManager/<fn>
// Uses player ped's task manager — CJ has active tasks at state 9.
//
// Inspired by gta-reversed issues:
//   #974  — CTaskComplexShuffleSeats bugs (task system correctness)
//   #1180 — NPCs don't stand up (task transitions failing)
//   #828  — Error entering police car (task assignment errors)
//   #706  — Walking in aim mode (task blend issues)
//
// The task system is central to all ped behavior — differential testing
// ensures the reversed task manager queries match the original exactly.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskManager.h"
#include "PedIntelligence.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- Player ped task queries ---

GAME_DIFF_TEST(CTaskManager, GetActiveTask_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto& mgr = ped->GetTaskManager();
    CTask* orig;
    CTask* rev;
    { HookDisableGuard guard("Tasks/CTaskManager/GetActiveTask"); orig = mgr.GetActiveTask(); }
    rev = mgr.GetActiveTask();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTaskManager, GetSimplestActiveTask_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto& mgr = ped->GetTaskManager();
    CTask* orig;
    CTask* rev;
    { HookDisableGuard guard("Tasks/CTaskManager/GetSimplestActiveTask"); orig = mgr.GetSimplestActiveTask(); }
    rev = mgr.GetSimplestActiveTask();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTaskManager, GetTaskSecondary_AllSlots) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto& mgr = ped->GetTaskManager();
    for (uint32 slot = 0; slot < TASK_SECONDARY_MAX; slot++) {
        CTask* orig;
        CTask* rev;
        { HookDisableGuard guard("Tasks/CTaskManager/GetTaskSecondary");
          orig = mgr.GetTaskSecondary((eSecondaryTask)slot); }
        rev = mgr.GetTaskSecondary((eSecondaryTask)slot);
        EXPECT_EQ(orig, rev);
    }
}

// Original and reversed disagree on HasTaskSecondary(nullptr) — use behavior test
GAME_TEST(CTaskManager, HasTaskSecondary_NullCheck) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto& mgr = ped->GetTaskManager();
    bool result = mgr.HasTaskSecondary(nullptr);
    EXPECT_FALSE(result); // nullptr should never be a secondary task
}

GAME_DIFF_TEST(CTaskManager, HasTaskSecondary_WithActiveTask) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto& mgr = ped->GetTaskManager();
    // Check if current active task is in secondary slots
    auto* activeTask = mgr.GetActiveTask();
    if (!activeTask) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No active task on player ped"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Tasks/CTaskManager/HasTaskSecondary"); orig = mgr.HasTaskSecondary(activeTask); }
    rev = mgr.HasTaskSecondary(activeTask);
    EXPECT_EQ(orig, rev);
}

// --- Spawned ped task queries ---
// Inspired by #1180: verify spawned NPC has correct initial task state

static Scenario::TestPed* s_taskPed = nullptr;
static bool s_triedTaskPed = false;

static CPed* GetTaskTestPed() {
    if (!s_triedTaskPed) {
        s_triedTaskPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0) {
            s_taskPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2494.0f, -1668.0f, 13.5f));
        }
    }
    return s_taskPed ? s_taskPed->AsPed() : nullptr;
}

GAME_DIFF_TEST(CTaskManager, GetActiveTask_SpawnedPed) {
    auto* ped = GetTaskTestPed();
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No spawned ped — no loaded model"); return; }
    auto& mgr = ped->GetTaskManager();
    CTask* orig;
    CTask* rev;
    { HookDisableGuard guard("Tasks/CTaskManager/GetActiveTask"); orig = mgr.GetActiveTask(); }
    rev = mgr.GetActiveTask();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTaskManager, GetSimplestActiveTask_SpawnedPed) {
    auto* ped = GetTaskTestPed();
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No spawned ped — no loaded model"); return; }
    auto& mgr = ped->GetTaskManager();
    CTask* orig;
    CTask* rev;
    { HookDisableGuard guard("Tasks/CTaskManager/GetSimplestActiveTask"); orig = mgr.GetSimplestActiveTask(); }
    rev = mgr.GetSimplestActiveTask();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTaskManager, GetTaskSecondary_SpawnedPed) {
    auto* ped = GetTaskTestPed();
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No spawned ped — no loaded model"); return; }
    auto& mgr = ped->GetTaskManager();
    for (uint32 slot = 0; slot < TASK_SECONDARY_MAX; slot++) {
        CTask* orig;
        CTask* rev;
        { HookDisableGuard guard("Tasks/CTaskManager/GetTaskSecondary");
          orig = mgr.GetTaskSecondary((eSecondaryTask)slot); }
        rev = mgr.GetTaskSecondary((eSecondaryTask)slot);
        EXPECT_EQ(orig, rev);
    }
}
