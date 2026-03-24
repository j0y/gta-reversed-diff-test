// test_CVehicle4.cpp — More differential tests for CVehicle.
// Hook paths: Vehicle/CVehicle/<fn>
//
// Tests vehicle functions not covered by previous CVehicle test files.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_veh4 = nullptr;
static bool s_triedVeh4 = false;

static CVehicle* GetVeh4() {
    if (!s_triedVeh4) {
        s_triedVeh4 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_veh4 = new Scenario::TestVehicle(model, CVector(2402.f, -1666.f, 13.5f));
    }
    return s_veh4 ? s_veh4->AsVehicle() : nullptr;
}

// --- CanBeDriven ---

GAME_DIFF_TEST(CVehicle4, CanBeDriven) {
    auto* veh = GetVeh4();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven");
      orig = veh->CanBeDriven(); }
    rev = veh->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

// --- CanPedLeanOut ---

GAME_DIFF_TEST(CVehicle4, CanPedLeanOut) {
    auto* veh = GetVeh4();
    auto* player = FindPlayerPed(0);
    if (!veh || !player) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedLeanOut");
      orig = veh->CanPedLeanOut(player); }
    rev = veh->CanPedLeanOut(player);
    EXPECT_EQ(orig, rev);
}

// --- UsesSiren ---

GAME_DIFF_TEST(CVehicle4, UsesSiren) {
    auto* veh = GetVeh4();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/UsesSiren");
      orig = veh->UsesSiren(); }
    rev = veh->UsesSiren();
    EXPECT_EQ(orig, rev);
}

// --- GetVehicleLightsStatus ---

GAME_DIFF_TEST(CVehicle4, GetVehicleLightsStatus) {
    auto* veh = GetVeh4();
    if (!veh) return;
    uint32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleLightsStatus");
      orig = veh->GetVehicleLightsStatus(); }
    rev = veh->GetVehicleLightsStatus();
    EXPECT_EQ(orig, rev);
}

// --- GetUpgrade ---

GAME_DIFF_TEST(CVehicle4, GetUpgrade_AllSlots) {
    auto* veh = GetVeh4();
    if (!veh) return;
    for (int32 i = 0; i < 15; i++) {
        int32 orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/GetUpgrade");
          orig = veh->GetUpgrade(i); }
        rev = veh->GetUpgrade(i);
        EXPECT_EQ(orig, rev);
    }
}

// GetReplacementUpgrade — original crashes with OOB node indices on spawned vehicles. Skipped.

// --- GetSpareHasslePosId ---

GAME_DIFF_TEST(CVehicle4, GetSpareHasslePosId) {
    auto* veh = GetVeh4();
    if (!veh) return;
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetSpareHasslePosId");
      orig = veh->GetSpareHasslePosId(); }
    rev = veh->GetSpareHasslePosId();
    EXPECT_EQ(orig, rev);
}

// --- GetPlaneGunsAutoAimAngle ---

GAME_DIFF_TEST(CVehicle4, GetPlaneGunsAutoAimAngle) {
    auto* veh = GetVeh4();
    if (!veh) return;
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetPlaneGunsAutoAimAngle");
      orig = veh->GetPlaneGunsAutoAimAngle(); }
    rev = veh->GetPlaneGunsAutoAimAngle();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// --- GetFiringRateMultiplier ---

GAME_DIFF_TEST(CVehicle4, GetFiringRateMultiplier) {
    auto* veh = GetVeh4();
    if (!veh) return;
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetFiringRateMultiplier");
      orig = veh->GetFiringRateMultiplier(); }
    rev = veh->GetFiringRateMultiplier();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// --- GetPlaneGunsRateOfFire ---

GAME_DIFF_TEST(CVehicle4, GetPlaneGunsRateOfFire) {
    auto* veh = GetVeh4();
    if (!veh) return;
    uint32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetPlaneGunsRateOfFire");
      orig = veh->GetPlaneGunsRateOfFire(); }
    rev = veh->GetPlaneGunsRateOfFire();
    EXPECT_EQ(orig, rev);
}

// --- GetSpecialColModel (returns bool) ---

GAME_DIFF_TEST(CVehicle4, GetSpecialColModel) {
    auto* veh = GetVeh4();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetSpecialColModel");
      orig = veh->GetSpecialColModel(); }
    rev = veh->GetSpecialColModel();
    EXPECT_EQ(orig, rev);
}

// --- IsSphereTouchingVehicle (takes CVector + float) ---

GAME_DIFF_TEST(CVehicle4, IsSphereTouchingVehicle_AtPos) {
    auto* veh = GetVeh4();
    if (!veh) return;
    CVector pos = veh->GetPosition();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsSphereTouchingVehicle");
      orig = veh->IsSphereTouchingVehicle(pos, 5.0f); }
    rev = veh->IsSphereTouchingVehicle(pos, 5.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CVehicle4, IsSphereTouchingVehicle_FarAway) {
    auto* veh = GetVeh4();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsSphereTouchingVehicle");
      orig = veh->IsSphereTouchingVehicle(CVector(0.f, 0.f, 0.f), 1.0f); }
    rev = veh->IsSphereTouchingVehicle(CVector(0.f, 0.f, 0.f), 1.0f);
    EXPECT_EQ(orig, rev);
}

// IsPassenger / IsDriver — overloaded hooks, path not found at runtime. Skipped.
