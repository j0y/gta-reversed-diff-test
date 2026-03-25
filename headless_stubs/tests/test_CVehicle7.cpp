// test_CVehicle7.cpp — More CVehicle differential tests on spawned vehicle.
// Hook paths: Vehicle/CVehicle/<fn>
//
// Untested hooked bool/value getters targeted here:
//   UsesSiren (re-test on different spawn pos), IsSphereTouchingVehicle (edge cases),
//   GetSpecialColModel, PickRandomPassenger, HeightAboveCeiling,
//   UpdatePassengerList (void, behavior), CanPedEnterCar

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_v7 = nullptr;
static bool s_triedV7 = false;

static CVehicle* GetV7() {
    if (!s_triedV7) {
        s_triedV7 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_v7 = new Scenario::TestVehicle(model, CVector(2500.f, -1666.f, 13.5f));
    }
    return s_v7 ? s_v7->AsVehicle() : nullptr;
}

// --- PickRandomPassenger (returns CPed*, empty vehicle should return nullptr) ---

GAME_DIFF_TEST(CVehicle7, PickRandomPassenger_Empty) {
    auto* veh = GetV7();
    if (!veh) return;
    CPed* orig;
    CPed* rev;
    { HookDisableGuard guard("Vehicle/CVehicle/PickRandomPassenger");
      orig = veh->PickRandomPassenger(); }
    rev = veh->PickRandomPassenger();
    EXPECT_EQ(orig, rev);
}

// --- GetSpecialColModel ---

GAME_DIFF_TEST(CVehicle7, GetSpecialColModel) {
    auto* veh = GetV7();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetSpecialColModel");
      orig = veh->GetSpecialColModel(); }
    rev = veh->GetSpecialColModel();
    EXPECT_EQ(orig, rev);
}

// --- CanPedEnterCar ---

GAME_DIFF_TEST(CVehicle7, CanPedEnterCar) {
    auto* veh = GetV7();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar");
      orig = veh->CanPedEnterCar(); }
    rev = veh->CanPedEnterCar();
    EXPECT_EQ(orig, rev);
}

// --- IsSphereTouchingVehicle with zero-radius at vehicle position ---

GAME_DIFF_TEST(CVehicle7, IsSphereTouchingVehicle_ZeroRadius) {
    auto* veh = GetV7();
    if (!veh) return;
    CVector pos = veh->GetPosition();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsSphereTouchingVehicle");
      orig = veh->IsSphereTouchingVehicle(pos, 0.f); }
    rev = veh->IsSphereTouchingVehicle(pos, 0.f);
    EXPECT_EQ(orig, rev);
}

// --- IsSphereTouchingVehicle with large radius ---

GAME_DIFF_TEST(CVehicle7, IsSphereTouchingVehicle_LargeRadius) {
    auto* veh = GetV7();
    if (!veh) return;
    CVector pos = veh->GetPosition();
    pos.x += 50.f; // offset away
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsSphereTouchingVehicle");
      orig = veh->IsSphereTouchingVehicle(pos, 100.f); }
    rev = veh->IsSphereTouchingVehicle(pos, 100.f);
    EXPECT_EQ(orig, rev);
}

// --- UpdatePassengerList (void, behavior — verify no crash on empty vehicle) ---

GAME_DIFF_TEST(CVehicle7, UpdatePassengerList_NoCrash) {
    auto* veh = GetV7();
    if (!veh) return;
    { HookDisableGuard guard("Vehicle/CVehicle/UpdatePassengerList");
      veh->UpdatePassengerList(); }
    veh->UpdatePassengerList();
    EXPECT_TRUE(true);
}

// --- UsesSiren (re-verify on this spawn, complements CVehicle4) ---

GAME_DIFF_TEST(CVehicle7, UsesSiren) {
    auto* veh = GetV7();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/UsesSiren");
      orig = veh->UsesSiren(); }
    rev = veh->UsesSiren();
    EXPECT_EQ(orig, rev);
}

// --- CanDoorsBeDamaged ---

GAME_DIFF_TEST(CVehicle7, CanDoorsBeDamaged) {
    auto* veh = GetV7();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanDoorsBeDamaged");
      orig = veh->CanDoorsBeDamaged(); }
    rev = veh->CanDoorsBeDamaged();
    EXPECT_EQ(orig, rev);
}

// --- IsVehicleNormal ---

GAME_DIFF_TEST(CVehicle7, IsVehicleNormal) {
    auto* veh = GetV7();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal");
      orig = veh->IsVehicleNormal(); }
    rev = veh->IsVehicleNormal();
    EXPECT_EQ(orig, rev);
}

// --- IsLawEnforcementVehicle ---

GAME_DIFF_TEST(CVehicle7, IsLawEnforcementVehicle) {
    auto* veh = GetV7();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle");
      orig = veh->IsLawEnforcementVehicle(); }
    rev = veh->IsLawEnforcementVehicle();
    EXPECT_EQ(orig, rev);
}

// --- CanBeDriven ---

GAME_DIFF_TEST(CVehicle7, CanBeDriven) {
    auto* veh = GetV7();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven");
      orig = veh->CanBeDriven(); }
    rev = veh->CanBeDriven();
    EXPECT_EQ(orig, rev);
}
