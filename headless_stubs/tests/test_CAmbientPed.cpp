// test_CAmbientPed.cpp — Differential tests on a spawned civilian ped.
// Uses ScenarioHelpers to create a real ped from a loaded model.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_ped = nullptr;
static bool s_triedCreate = false;

static CPed* GetTestPed() {
    if (!s_triedCreate) {
        s_triedCreate = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0) {
            s_ped = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2492.0f, -1665.0f, 13.5f));
        }
    }
    return s_ped ? s_ped->AsPed() : nullptr;
}

#define DIFF_SPAWNED_PED_BOOL(FnName) \
    GAME_DIFF_TEST(CSpawnedPed, FnName) { \
        auto* ped = GetTestPed(); \
        if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped — no loaded model"); return; } \
        bool orig, rev; \
        { HookDisableGuard guard("Entity/Ped/CPed/" #FnName); orig = ped->FnName(); } \
        rev = ped->FnName(); \
        EXPECT_EQ(orig, rev); \
    }

DIFF_SPAWNED_PED_BOOL(IsAlive)
DIFF_SPAWNED_PED_BOOL(IsPlayer)
DIFF_SPAWNED_PED_BOOL(IsPedInControl)
DIFF_SPAWNED_PED_BOOL(IsPedShootable)
DIFF_SPAWNED_PED_BOOL(CanBeDeleted)
DIFF_SPAWNED_PED_BOOL(CanPedReturnToState)
DIFF_SPAWNED_PED_BOOL(CanSetPedState)
DIFF_SPAWNED_PED_BOOL(IsWearingGoggles)
DIFF_SPAWNED_PED_BOOL(CanPedHoldConversation)
DIFF_SPAWNED_PED_BOOL(UseGroundColModel)
DIFF_SPAWNED_PED_BOOL(CanBeArrested)
DIFF_SPAWNED_PED_BOOL(CanUseTorsoWhenLooking)

GAME_DIFF_TEST(CSpawnedPed, GetBikeRidingSkill) {
    auto* ped = GetTestPed();
    if (!ped) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetBikeRidingSkill"); orig = ped->GetBikeRidingSkill(); }
    rev = ped->GetBikeRidingSkill();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CSpawnedPed, GetWalkAnimSpeed) {
    auto* ped = GetTestPed();
    if (!ped) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetWalkAnimSpeed"); orig = ped->GetWalkAnimSpeed(); }
    rev = ped->GetWalkAnimSpeed();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// Entity queries on spawned ped
GAME_DIFF_TEST(CSpawnedPed, Entity_GetDistToBase) {
    auto* ped = GetTestPed();
    if (!ped) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetDistanceFromCentreOfMassToBaseOfModel");
      orig = ped->GetDistanceFromCentreOfMassToBaseOfModel(); }
    rev = ped->GetDistanceFromCentreOfMassToBaseOfModel();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CSpawnedPed, Entity_GetColModel) {
    auto* ped = GetTestPed();
    if (!ped) return;
    CColModel* orig;
    CColModel* rev;
    { HookDisableGuard guard("Entity/CEntity/GetColModel"); orig = ped->GetColModel(); }
    rev = ped->GetColModel();
    EXPECT_EQ(orig, rev);
}

// Physical queries
GAME_DIFF_TEST(CSpawnedPed, Physical_CanBeDamaged) {
    auto* ped = GetTestPed();
    if (!ped) return;
    auto* phys = static_cast<CPhysical*>(ped);
    bool orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/CanPhysicalBeDamaged");
      orig = phys->CanPhysicalBeDamaged(WEAPON_UNARMED, nullptr); }
    rev = phys->CanPhysicalBeDamaged(WEAPON_UNARMED, nullptr);
    EXPECT_EQ(orig, rev);
}

// Diagnostic
GAME_TEST(CSpawnedPed, SpawnDiagnostic) {
    auto* ped = GetTestPed();
    char buf[256];
    if (ped) {
        snprintf(buf, sizeof(buf), "[DIAG] Spawned ped: model=%d alive=%d state=%d health=%.0f pos=(%.1f,%.1f,%.1f)",
            ped->GetModelIndex(), ped->IsAlive(), (int)ped->GetPedState(),
            ped->m_fHealth, ped->GetPosition().x, ped->GetPosition().y, ped->GetPosition().z);
    } else {
        int32 model = Scenario::FindLoadedPedModel();
        snprintf(buf, sizeof(buf), "[DIAG] Failed to spawn ped. FindLoadedPedModel=%d", model);
    }
    { FILE* f = fopen("C:\\ped_spawn_diag.log", "w"); if(f) { fputs(buf, f); fputc('\n', f); fclose(f); } }
    EXPECT_TRUE(ped != nullptr);
}
