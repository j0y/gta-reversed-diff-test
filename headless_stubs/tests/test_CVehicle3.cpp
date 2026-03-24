// test_CVehicle3.cpp — Extended differential tests for CVehicle.
// Hook paths: Vehicle/CVehicle/<fn>
//
// Tests vehicle queries on a spawned vehicle — functions not covered
// by test_CVehicle.cpp or test_CVehicle2.cpp.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_veh3 = nullptr;
static bool s_triedVeh3 = false;

static CVehicle* GetVeh3() {
    if (!s_triedVeh3) {
        s_triedVeh3 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_veh3 = new Scenario::TestVehicle(model, CVector(2404.f, -1668.f, 13.5f));
    }
    return s_veh3 ? s_veh3->AsVehicle() : nullptr;
}

// --- GetRemapIndex ---

GAME_DIFF_TEST(CVehicle3, GetRemapIndex) {
    auto* veh = GetVeh3();
    if (!veh) return;
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetRemapIndex");
      orig = veh->GetRemapIndex(); }
    rev = veh->GetRemapIndex();
    EXPECT_EQ(orig, rev);
}

// --- GetVehicleAppearance ---

GAME_DIFF_TEST(CVehicle3, GetVehicleAppearance) {
    auto* veh = GetVeh3();
    if (!veh) return;
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance");
      orig = (int32)veh->GetVehicleAppearance(); }
    rev = (int32)veh->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

// --- CanBeDeleted ---

GAME_DIFF_TEST(CVehicle3, CanBeDeleted) {
    auto* veh = GetVeh3();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDeleted");
      orig = veh->CanBeDeleted(); }
    rev = veh->CanBeDeleted();
    EXPECT_EQ(orig, rev);
}

// --- IsUpsideDown ---

GAME_DIFF_TEST(CVehicle3, IsUpsideDown) {
    auto* veh = GetVeh3();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsUpsideDown");
      orig = veh->IsUpsideDown(); }
    rev = veh->IsUpsideDown();
    EXPECT_EQ(orig, rev);
}

// --- IsOnItsSide ---

GAME_DIFF_TEST(CVehicle3, IsOnItsSide) {
    auto* veh = GetVeh3();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsOnItsSide");
      orig = veh->IsOnItsSide(); }
    rev = veh->IsOnItsSide();
    EXPECT_EQ(orig, rev);
}

// --- CanPedOpenLocks ---

GAME_DIFF_TEST(CVehicle3, CanPedOpenLocks) {
    auto* veh = GetVeh3();
    if (!veh) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedOpenLocks");
      orig = veh->CanPedOpenLocks(player); }
    rev = veh->CanPedOpenLocks(player);
    EXPECT_EQ(orig, rev);
}

// --- CanDoorsBeDamaged ---

GAME_DIFF_TEST(CVehicle3, CanDoorsBeDamaged) {
    auto* veh = GetVeh3();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanDoorsBeDamaged");
      orig = veh->CanDoorsBeDamaged(); }
    rev = veh->CanDoorsBeDamaged();
    EXPECT_EQ(orig, rev);
}

// --- CanPedEnterCar ---

GAME_DIFF_TEST(CVehicle3, CanPedEnterCar) {
    auto* veh = GetVeh3();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar");
      orig = veh->CanPedEnterCar(); }
    rev = veh->CanPedEnterCar();
    EXPECT_EQ(orig, rev);
}

// --- IsVehicleNormal ---

GAME_DIFF_TEST(CVehicle3, IsVehicleNormal) {
    auto* veh = GetVeh3();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal");
      orig = veh->IsVehicleNormal(); }
    rev = veh->IsVehicleNormal();
    EXPECT_EQ(orig, rev);
}

// --- IsLawEnforcementVehicle ---

GAME_DIFF_TEST(CVehicle3, IsLawEnforcementVehicle) {
    auto* veh = GetVeh3();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle");
      orig = veh->IsLawEnforcementVehicle(); }
    rev = veh->IsLawEnforcementVehicle();
    EXPECT_EQ(orig, rev);
}

// --- ChangeLawEnforcerState (query-like — checks flag and model) ---

GAME_DIFF_TEST(CVehicle3, ChangeLawEnforcerState_Unchanged) {
    auto* veh = GetVeh3();
    if (!veh) return;
    // Save state
    bool savedState = veh->vehicleFlags.bIsLawEnforcer;
    // Toggle to false then back
    { HookDisableGuard guard("Vehicle/CVehicle/ChangeLawEnforcerState");
      veh->ChangeLawEnforcerState(false); }
    bool origResult = veh->vehicleFlags.bIsLawEnforcer;
    veh->ChangeLawEnforcerState(false);
    bool revResult = veh->vehicleFlags.bIsLawEnforcer;
    EXPECT_EQ(origResult, revResult);
    // Restore
    veh->ChangeLawEnforcerState(savedState);
}

// --- GetPlaneNumGuns (noted with "register problem?" comment upstream) ---

GAME_DIFF_TEST(CVehicle3, GetPlaneNumGuns) {
    auto* veh = GetVeh3();
    if (!veh) return;
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetPlaneNumGuns");
      orig = veh->GetPlaneNumGuns(); }
    rev = veh->GetPlaneNumGuns();
    EXPECT_EQ(orig, rev);
}
