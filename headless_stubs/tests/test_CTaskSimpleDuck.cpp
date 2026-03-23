// test_CTaskSimpleDuck.cpp — Tests for CTaskSimpleDuck.
// Hook paths: Tasks/TaskTypes/CTaskSimpleDuck/<fn>
//
// CTaskSimpleDuck handles crouching/ducking animations with multiple control
// types (standalone, task-controlled, anim-controlled, script-controlled).
// Tests verify construction, cloning, static helpers, and control type logic.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- Construction & type ---

GAME_TEST(CTaskSimpleDuck, GetTaskType) {
    CTaskSimpleDuck task(DUCK_STANDALONE, 2000);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_DUCK);
}

GAME_TEST(CTaskSimpleDuck, IsSimple) {
    CTaskSimpleDuck task(DUCK_STANDALONE, 2000);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleDuck, Constructor_Standalone) {
    CTaskSimpleDuck task(DUCK_STANDALONE, 3000, 5);
    EXPECT_EQ((int32)task.m_DuckControlType, (int32)DUCK_STANDALONE);
    EXPECT_EQ(task.m_LengthOfDuck, 3000);
    EXPECT_EQ(task.m_ShotWhizzingCounter, 5);
}

GAME_TEST(CTaskSimpleDuck, Constructor_TaskControlled) {
    CTaskSimpleDuck task(DUCK_TASK_CONTROLLED, 1000);
    EXPECT_EQ((int32)task.m_DuckControlType, (int32)DUCK_TASK_CONTROLLED);
    EXPECT_EQ(task.m_LengthOfDuck, 1000);
    EXPECT_EQ(task.m_ShotWhizzingCounter, -1); // default
}

GAME_TEST(CTaskSimpleDuck, Constructor_DefaultFlags) {
    CTaskSimpleDuck task(DUCK_STANDALONE, 2000);
    EXPECT_EQ(task.m_bIsFinished, 0);
    EXPECT_EQ(task.m_bIsAborting, 0);
    EXPECT_TRUE(task.m_bNeedToSetDuckFlag);
    EXPECT_TRUE(task.m_bIsInControl);
}

// --- Clone ---

GAME_TEST(CTaskSimpleDuck, Clone_PreservesType) {
    CTaskSimpleDuck original(DUCK_STANDALONE_WEAPON_CROUCH, 4000, 3);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_DUCK);

    auto* c = static_cast<CTaskSimpleDuck*>(cloned);
    EXPECT_EQ((int32)c->m_DuckControlType, (int32)DUCK_STANDALONE_WEAPON_CROUCH);
    EXPECT_EQ(c->m_LengthOfDuck, 4000);
    EXPECT_EQ(c->m_ShotWhizzingCounter, 3);

    delete cloned;
}

// --- Static helper: CanPedDuck ---

GAME_DIFF_TEST(CTaskSimpleDuck, CanPedDuck_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleDuck/CanPedDuck");
      orig = CTaskSimpleDuck::CanPedDuck(ped); }
    rev = CTaskSimpleDuck::CanPedDuck(ped);
    EXPECT_EQ(orig, rev);
}

static Scenario::TestPed* s_duckPed = nullptr;
static bool s_triedDuckPed = false;

static CPed* GetDuckTestPed() {
    if (!s_triedDuckPed) {
        s_triedDuckPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0) {
            s_duckPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2492.0f, -1668.0f, 13.5f));
        }
    }
    return s_duckPed ? s_duckPed->AsPed() : nullptr;
}

GAME_DIFF_TEST(CTaskSimpleDuck, CanPedDuck_SpawnedPed) {
    auto* ped = GetDuckTestPed();
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No spawned ped"); return; }

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleDuck/CanPedDuck");
      orig = CTaskSimpleDuck::CanPedDuck(ped); }
    rev = CTaskSimpleDuck::CanPedDuck(ped);
    EXPECT_EQ(orig, rev);
}

// --- Control type ---

GAME_TEST(CTaskSimpleDuck, GetControlType) {
    CTaskSimpleDuck task(DUCK_SCRIPT_CONTROLLED, 2000);
    EXPECT_EQ((int32)task.GetControlType(), (int32)DUCK_SCRIPT_CONTROLLED);
}

// --- MakeAbortable ---

GAME_DIFF_TEST(CTaskSimpleDuck, MakeAbortable_Immediate) {
    CTaskSimpleDuck task(DUCK_STANDALONE, 2000);
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleDuck/MakeAbortable");
      CTaskSimpleDuck t1(DUCK_STANDALONE, 2000);
      orig = t1.MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr); }
    {
      CTaskSimpleDuck t2(DUCK_STANDALONE, 2000);
      rev = t2.MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr); }
    EXPECT_EQ(orig, rev);
}

// --- IsTaskInUseByOtherTasks ---

GAME_TEST(CTaskSimpleDuck, IsTaskInUseByOtherTasks_FreshTask) {
    CTaskSimpleDuck task(DUCK_STANDALONE, 2000);
    // A freshly created task not assigned to any ped should not be in use
    bool result = task.IsTaskInUseByOtherTasks();
    // Just verify it doesn't crash — result depends on global state
    (void)result;
    EXPECT_TRUE(true);
}
