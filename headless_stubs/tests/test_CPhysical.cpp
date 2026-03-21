// test_CPhysical.cpp — Differential tests for CPhysical methods.
// Hook paths: Entity/CPhysical/<fn>
// Tests on player ped and spawned vehicle — both inherit from CPhysical.
//
// Inspired by gta-reversed issues:
//   #1145 — Vehicle physics state correctness
//   Physics layer is critical for vehicle/ped movement fidelity

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Entity/Physical.h"

// --- Player ped physics ---

GAME_DIFF_TEST(CPhysical, GetSpeed_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto* phys = static_cast<CPhysical*>(ped);
    CVector point(0.0f, 0.0f, 0.0f);
    CVector orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetSpeed"); orig = phys->GetSpeed(point); }
    rev = phys->GetSpeed(point);
    EXPECT_VEC_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPhysical, GetSpeed_Player_Offset) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto* phys = static_cast<CPhysical*>(ped);
    // Test with an offset point (e.g., head position relative to center of mass)
    CVector offsets[] = {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.5f, 0.5f, 0.5f },
    };
    for (auto& pt : offsets) {
        CVector orig, rev;
        { HookDisableGuard guard("Entity/CPhysical/GetSpeed"); orig = phys->GetSpeed(pt); }
        rev = phys->GetSpeed(pt);
        EXPECT_VEC_NEAR(orig, rev, 1e-5f);
    }
}

GAME_DIFF_TEST(CPhysical, GetHasCollidedWithAnyObject_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto* phys = static_cast<CPhysical*>(ped);
    bool orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetHasCollidedWithAnyObject");
      orig = phys->GetHasCollidedWithAnyObject(); }
    rev = phys->GetHasCollidedWithAnyObject();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPhysical, CanPhysicalBeDamaged_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto* phys = static_cast<CPhysical*>(ped);
    eWeaponType weapons[] = {
        WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_SHOTGUN,
        WEAPON_AK47, WEAPON_KNIFE, WEAPON_FLAMETHROWER
    };
    for (auto wt : weapons) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/CPhysical/CanPhysicalBeDamaged");
          orig = phys->CanPhysicalBeDamaged(wt, nullptr); }
        rev = phys->CanPhysicalBeDamaged(wt, nullptr);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPhysical, GetLightingTotal_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto* phys = static_cast<CPhysical*>(ped);
    float orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetLightingTotal"); orig = phys->GetLightingTotal(); }
    rev = phys->GetLightingTotal();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CPhysical, GetLightingFromCol_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto* phys = static_cast<CPhysical*>(ped);
    for (bool interior : { false, true }) {
        float orig, rev;
        { HookDisableGuard guard("Entity/CPhysical/GetLightingFromCol");
          orig = phys->GetLightingFromCol(interior); }
        rev = phys->GetLightingFromCol(interior);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- Spawned vehicle physics ---

static Scenario::TestVehicle* s_physVeh = nullptr;
static bool s_triedPhysVeh = false;

static CPhysical* GetPhysVehicle() {
    if (!s_triedPhysVeh) {
        s_triedPhysVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_physVeh = new Scenario::TestVehicle(model, CVector(2502.0f, -1665.0f, 13.5f));
        }
    }
    return s_physVeh ? static_cast<CPhysical*>(s_physVeh->AsVehicle()) : nullptr;
}

GAME_DIFF_TEST(CPhysical, GetSpeed_Vehicle) {
    auto* phys = GetPhysVehicle();
    if (!phys) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle — no loaded model"); return; }
    CVector point(0.0f, 0.0f, 0.0f);
    CVector orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetSpeed"); orig = phys->GetSpeed(point); }
    rev = phys->GetSpeed(point);
    EXPECT_VEC_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPhysical, GetHasCollidedWithAnyObject_Vehicle) {
    auto* phys = GetPhysVehicle();
    if (!phys) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle — no loaded model"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetHasCollidedWithAnyObject");
      orig = phys->GetHasCollidedWithAnyObject(); }
    rev = phys->GetHasCollidedWithAnyObject();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPhysical, CanPhysicalBeDamaged_Vehicle) {
    auto* phys = GetPhysVehicle();
    if (!phys) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle — no loaded model"); return; }
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_AK47, WEAPON_FLAMETHROWER };
    for (auto wt : weapons) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/CPhysical/CanPhysicalBeDamaged");
          orig = phys->CanPhysicalBeDamaged(wt, nullptr); }
        rev = phys->CanPhysicalBeDamaged(wt, nullptr);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPhysical, GetLightingTotal_Vehicle) {
    auto* phys = GetPhysVehicle();
    if (!phys) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle — no loaded model"); return; }
    float orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetLightingTotal"); orig = phys->GetLightingTotal(); }
    rev = phys->GetLightingTotal();
    EXPECT_NEAR(orig, rev, 1e-3f);
}
