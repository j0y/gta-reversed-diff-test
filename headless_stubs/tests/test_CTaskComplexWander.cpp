// test_CTaskComplexWander.cpp — Tests for CTaskComplexWander.
// Hook paths: Tasks/TaskTypes/CTaskComplexWander/<fn>
//
// CTaskComplexWander is the abstract base for all wander behaviors (Standard,
// Cop, Medic, Criminal, Gang, Shop, Flee, Prostitute). Tests exercise the
// base class methods: ValidNodes, ComputeTargetHeading, GetWanderTaskByPedType,
// and flag management.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Tasks/TaskTypes/TaskComplexWanderStandard.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// CTaskComplexWander is abstract — use CTaskComplexWanderStandard as concrete subclass

// --- Construction & type ---

GAME_TEST(CTaskComplexWander, GetTaskType) {
    CTaskComplexWanderStandard task(PEDMOVE_WALK, 0, true);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WANDER);
}

GAME_TEST(CTaskComplexWander, IsComplex) {
    CTaskComplexWanderStandard task(PEDMOVE_WALK, 0);
    EXPECT_FALSE(task.IsSimple());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexWander, Constructor_Fields) {
    CTaskComplexWanderStandard task(PEDMOVE_RUN, 3, false);
    EXPECT_EQ((int32)task.m_nMoveState, (int32)PEDMOVE_RUN);
    EXPECT_EQ(task.m_nDir, 3);
    EXPECT_FALSE(task.m_bWanderSensibly);
}

GAME_TEST(CTaskComplexWander, Constructor_Defaults) {
    CTaskComplexWanderStandard task(PEDMOVE_WALK, 0);
    EXPECT_TRUE(task.m_bWanderSensibly);
    EXPECT_NEAR(task.m_fTargetRadius, 0.5f, 1e-5f);
}

// --- ValidNodes ---

GAME_TEST(CTaskComplexWander, ValidNodes_FreshTask) {
    CTaskComplexWanderStandard task(PEDMOVE_WALK, 0);
    // Fresh task has uninitialized node addresses — ValidNodes checks internal state
    bool result = task.ValidNodes();
    (void)result; // Just verify no crash
    EXPECT_TRUE(true);
}

// --- Direction values ---

GAME_TEST(CTaskComplexWander, DirectionRange) {
    // Direction should be 0-7 (8 compass directions)
    for (uint8 dir = 0; dir < 8; dir++) {
        CTaskComplexWanderStandard task(PEDMOVE_WALK, dir);
        EXPECT_EQ(task.m_nDir, dir);
    }
}

// --- GetWanderTaskByPedType differential ---

static Scenario::TestPed* s_wanderPed = nullptr;
static bool s_triedWanderPed = false;

static CPed* GetWanderTestPed() {
    if (!s_triedWanderPed) {
        s_triedWanderPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0) {
            s_wanderPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2490.0f, -1668.0f, 13.5f));
        }
    }
    return s_wanderPed ? s_wanderPed->AsPed() : nullptr;
}

GAME_DIFF_TEST(CTaskComplexWander, GetWanderTaskByPedType_Civilian) {
    auto* ped = GetWanderTestPed();
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No spawned ped"); return; }

    CTaskComplexWander* orig;
    CTaskComplexWander* rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexWander/GetWanderTaskByPedType");
      orig = CTaskComplexWander::GetWanderTaskByPedType(ped); }
    rev = CTaskComplexWander::GetWanderTaskByPedType(ped);

    // Both should return a wander task (or both null)
    if (orig && rev) {
        EXPECT_EQ((int32)orig->GetTaskType(), (int32)rev->GetTaskType());
        EXPECT_EQ((int32)orig->m_nMoveState, (int32)rev->m_nMoveState);
    } else {
        EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
    }

    // GetWanderTaskByPedType allocates — clean up
    delete orig;
    delete rev;
}

GAME_DIFF_TEST(CTaskComplexWander, GetWanderTaskByPedType_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CTaskComplexWander* orig;
    CTaskComplexWander* rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexWander/GetWanderTaskByPedType");
      orig = CTaskComplexWander::GetWanderTaskByPedType(ped); }
    rev = CTaskComplexWander::GetWanderTaskByPedType(ped);

    if (orig && rev) {
        EXPECT_EQ((int32)orig->GetTaskType(), (int32)rev->GetTaskType());
    } else {
        EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
    }

    delete orig;
    delete rev;
}

// --- ComputeTargetHeading differential ---

GAME_DIFF_TEST(CTaskComplexWander, ComputeTargetHeading) {
    auto* ped = GetWanderTestPed();
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No spawned ped"); return; }

    CTaskComplexWanderStandard task(PEDMOVE_WALK, 0);

    float orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexWander/ComputeTargetHeading");
      orig = task.ComputeTargetHeading(ped); }
    rev = task.ComputeTargetHeading(ped);
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// --- Wander type (subclass-specific) ---

GAME_TEST(CTaskComplexWander, WanderType_Standard) {
    CTaskComplexWanderStandard task(PEDMOVE_WALK, 0);
    EXPECT_EQ((int32)task.GetWanderType(), (int32)WANDER_TYPE_STANDARD);
}
