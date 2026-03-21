// test_Scenario_PedInVehicle.cpp — Scenario test: ped placed in a vehicle.
//
// Spawns a vehicle and a ped, puts the ped in the driver seat by setting
// the vehicle/ped state directly, then verifies all related queries return
// consistent results between reversed and original code.
//
// This is the most common game state transition — tests that vehicle occupant
// queries, ped vehicle queries, and entity relationship queries all agree.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"

// Helper: manually place a ped as driver without calling SetDriver
// (SetDriver has side effects like freebies, streaming requests)
static void PlacePedAsDriver(CVehicle* veh, CPed* ped) {
    veh->m_pDriver = ped;
    ped->m_pVehicle = veh;
    ped->bInVehicle = true;
}

static void RemovePedFromDriver(CVehicle* veh, CPed* ped) {
    veh->m_pDriver = nullptr;
    ped->m_pVehicle = nullptr;
    ped->bInVehicle = false;
}

// Shared test objects
static Scenario::TestVehicle* s_scVeh = nullptr;
static Scenario::TestPed* s_scPed = nullptr;
static bool s_scSetup = false;

static bool SetupScenario() {
    if (s_scSetup) return (s_scVeh && s_scPed);
    s_scSetup = true;

    int32 vehModel = Scenario::FindLoadedVehicleModel();
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (vehModel < 0 || pedModel < 0) return false;

    CVector pos = CVector(2470.0f, -1665.0f, 13.5f);
    s_scVeh = new Scenario::TestVehicle(vehModel, pos);
    s_scPed = new Scenario::TestPed(PED_TYPE_CIVMALE, pedModel, pos);
    return (s_scVeh && *s_scVeh && s_scPed && *s_scPed);
}

#define SC_GUARD() if (!SetupScenario()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "Failed to setup scenario"); return; }

// --- Vehicle-side queries with driver ---

GAME_TEST(Scenario_PedInVehicle, VehicleHasDriver) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    PlacePedAsDriver(veh, ped);

    EXPECT_TRUE(veh->HasDriver());
    EXPECT_EQ((uintptr_t)veh->GetDriver(), (uintptr_t)ped);
    EXPECT_TRUE(veh->IsDriver(ped));
    EXPECT_FALSE(veh->IsPassenger(ped));

    RemovePedFromDriver(veh, ped);
}

GAME_TEST(Scenario_PedInVehicle, PedIsInVehicle) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    PlacePedAsDriver(veh, ped);

    EXPECT_TRUE(ped->IsInVehicle());
    EXPECT_TRUE(ped->IsInVehicle(veh));
    EXPECT_EQ((uintptr_t)ped->GetVehicleIfInOne(), (uintptr_t)veh);

    RemovePedFromDriver(veh, ped);
}

GAME_TEST(Scenario_PedInVehicle, CanBeDriven_WithDriver) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    // Without driver
    bool canDriveEmpty = veh->CanBeDriven();

    // With driver
    PlacePedAsDriver(veh, ped);
    bool canDriveOccupied = veh->CanBeDriven();

    RemovePedFromDriver(veh, ped);

    // Both should be true for a normal healthy vehicle
    EXPECT_TRUE(canDriveEmpty);
    EXPECT_TRUE(canDriveOccupied);
}

// --- Differential tests with driver present ---

GAME_DIFF_TEST(Scenario_PedInVehicle, Diff_IsLawEnforcement_WithDriver) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    PlacePedAsDriver(veh, ped);

    bool rev = veh->IsLawEnforcementVehicle();
    bool orig;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle");
      orig = veh->IsLawEnforcementVehicle(); }
    EXPECT_EQ(rev, orig);

    RemovePedFromDriver(veh, ped);
}

GAME_DIFF_TEST(Scenario_PedInVehicle, Diff_CanPedEnterCar_Occupied) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    PlacePedAsDriver(veh, ped);

    // Can the player enter a car that already has a driver?
    bool rev = veh->CanPedEnterCar();
    bool orig;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar");
      orig = veh->CanPedEnterCar(); }
    EXPECT_EQ(rev, orig);

    RemovePedFromDriver(veh, ped);
}

GAME_DIFF_TEST(Scenario_PedInVehicle, Diff_CanPedOpenLocks_Occupied) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();
    auto* player = FindPlayerPed(0);
    if (!player) return;

    PlacePedAsDriver(veh, ped);

    bool rev = veh->CanPedOpenLocks(player);
    bool orig;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedOpenLocks");
      orig = veh->CanPedOpenLocks(player); }
    EXPECT_EQ(rev, orig);

    RemovePedFromDriver(veh, ped);
}

GAME_DIFF_TEST(Scenario_PedInVehicle, Diff_IsVehicleNormal_Occupied) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    PlacePedAsDriver(veh, ped);

    bool rev = veh->IsVehicleNormal();
    bool orig;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal");
      orig = veh->IsVehicleNormal(); }
    EXPECT_EQ(rev, orig);

    RemovePedFromDriver(veh, ped);
}

GAME_DIFF_TEST(Scenario_PedInVehicle, Diff_CanBeDeleted_Occupied) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    PlacePedAsDriver(veh, ped);

    bool rev = veh->CanBeDeleted();
    bool orig;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDeleted");
      orig = veh->CanBeDeleted(); }
    EXPECT_EQ(rev, orig);

    RemovePedFromDriver(veh, ped);
}

// --- Ped-side queries while in vehicle ---

GAME_DIFF_TEST(Scenario_PedInVehicle, Diff_Ped_IsAlive) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    PlacePedAsDriver(veh, ped);

    bool rev = ped->IsAlive();
    bool orig;
    { HookDisableGuard guard("Entity/Ped/CPed/IsAlive");
      orig = ped->IsAlive(); }
    EXPECT_EQ(rev, orig);

    RemovePedFromDriver(veh, ped);
}

GAME_DIFF_TEST(Scenario_PedInVehicle, Diff_Ped_IsPedInControl) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    PlacePedAsDriver(veh, ped);

    bool rev = ped->IsPedInControl();
    bool orig;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedInControl");
      orig = ped->IsPedInControl(); }
    EXPECT_EQ(rev, orig);

    RemovePedFromDriver(veh, ped);
}

// --- State transitions ---

GAME_TEST(Scenario_PedInVehicle, StateTransition_InOut) {
    SC_GUARD()
    auto* veh = s_scVeh->AsVehicle();
    auto* ped = s_scPed->AsPed();

    // Before: not in vehicle
    EXPECT_FALSE(ped->IsInVehicle());
    EXPECT_FALSE(veh->HasDriver());

    // Place in
    PlacePedAsDriver(veh, ped);
    EXPECT_TRUE(ped->IsInVehicle());
    EXPECT_TRUE(veh->HasDriver());
    EXPECT_TRUE(veh->IsDriver(ped));

    // Remove
    RemovePedFromDriver(veh, ped);
    EXPECT_FALSE(ped->IsInVehicle());
    EXPECT_FALSE(veh->HasDriver());
}
