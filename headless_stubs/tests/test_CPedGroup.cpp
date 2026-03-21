// test_CPedGroup.cpp — Differential tests for CPedGroup member functions.
// Hook paths: Global/CPedGroup/<fn>
// Uses player ped and spawned vehicle for group queries.
//
// Inspired by gta-reversed issues:
//   #974 — CTaskComplexShuffleSeats related bugs (group/seat interactions)
//   #828 — Error entering police car with officer inside

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "PedGroup.h"
#include "PedGroups.h"

// CPedGroupMembership exposes GetMember — we test via the CPedGroup interface

GAME_DIFF_TEST(CPedGroup, IsAnyoneUsingCar_NullVehicle) {
    CPedGroup group;
    bool orig, rev;
    // Passing nullptr should be safe and return false
    { HookDisableGuard guard("Global/CPedGroup/IsAnyoneUsingCar"); orig = group.IsAnyoneUsingCar(nullptr); }
    rev = group.IsAnyoneUsingCar(nullptr);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedGroup, IsAnyoneUsingCar_SpawnedVehicle) {
    // Use a fresh group (empty) — no one should be using the car
    CPedGroup group;
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No loaded vehicle model"); return; }
    Scenario::TestVehicle veh(vehModel, CVector(2490.0f, -1670.0f, 13.5f));
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "Failed to spawn vehicle"); return; }

    bool orig, rev;
    { HookDisableGuard guard("Global/CPedGroup/IsAnyoneUsingCar"); orig = group.IsAnyoneUsingCar(veh.AsVehicle()); }
    rev = group.IsAnyoneUsingCar(veh.AsVehicle());
    EXPECT_EQ(orig, rev);
}

// GetClosestGroupPed on an empty group should return nullptr
GAME_DIFF_TEST(CPedGroup, GetClosestGroupPed_EmptyGroup) {
    CPedGroup group;
    auto* player = FindPlayerPed(0);
    if (!player) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    float distOrig = 0.f, distRev = 0.f;
    CPed* orig;
    CPed* rev;
    { HookDisableGuard guard("Global/CPedGroup/GetClosestGroupPed"); orig = group.GetClosestGroupPed(player, &distOrig); }
    rev = group.GetClosestGroupPed(player, &distRev);
    EXPECT_EQ(orig, rev);
}

// FindDistanceToNearestMember on an empty group
GAME_DIFF_TEST(CPedGroup, FindDistanceToNearestMember_EmptyGroup) {
    CPedGroup group;
    CPed* nearestOrig = nullptr;
    CPed* nearestRev = nullptr;
    float orig, rev;
    { HookDisableGuard guard("Global/CPedGroup/FindDistanceToNearestMember"); orig = group.FindDistanceToNearestMember(&nearestOrig); }
    rev = group.FindDistanceToNearestMember(&nearestRev);
    EXPECT_NEAR(orig, rev, 1e-3f);
    EXPECT_EQ(nearestOrig, nearestRev);
}
